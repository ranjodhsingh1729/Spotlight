/**
 * @file face.hpp
 * @author Ranjodh Singh
 *
 * @brief FACE.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef FACE_HPP
#define FACE_HPP

#include <cmath>
#include <string>
#include <limits>
#include <vector>

#include <spotlight/models/model.hpp>
#include <spotlight/models/face/face_utils.hpp>


namespace spotlight {

template <typename ModelType>
class FaceDetection
{
  // UltraFace Model Constants.
  static constexpr float size_variance = 0.2f;
  static constexpr float center_variance = 0.1f;
  static constexpr int num_boxes = 4;
  static constexpr int box_counts[] = {3, 2, 2, 3};
  static constexpr float strides[] = {8.f, 16.f, 32.f, 64.f};
  static constexpr float min_boxes[][3] = {
    { 10.f,  16.f,  24.f},
    { 32.f,  48.f,   0.f},
    { 64.f,  96.f,   0.f},
    {128.f, 192.f, 256.f}
  };

 public:
  FaceDetection(
    const int top_k,
    const float score_threshold,
    const float iou_threshold,
    const float temporal_alpha,
    const float jerk_tolerance,
    const std::string& model_path,
    const int num_threads
  )
    : top_k(top_k), score_threshold(score_threshold),
      iou_threshold(iou_threshold), temporal_alpha(temporal_alpha),
      jerk_tolerance(jerk_tolerance), model(model_path, num_threads)
  {
    scores_tensor = model.getOutputTensor(0);
    boxes_tensor = model.getOutputTensor(1);

    generate_priors();
    selections.assign(priors.size(), true);
    detections.assign(priors.size(), Detection{});

    face_frame = {
      0.f, 0.f, (float)ModelWidth(), (float)ModelHeight(), 0.f
    };
  }

  Detection invoke(const ModelType* input)
  {
    model.setInputTensor(input);
    model.invoke();
    return postProcess();
  }

  Detection postProcess()
  {
    const Point frame_center = face_frame.center();
    const int n = std::min(top_k, getDetections());

    int min_idx = -1;
    float min_dist = std::numeric_limits<float>::infinity();
    for (int i = 0; i < n; i++)
    {
      if (!selections[i])
        continue;

      const float cur_dist = frame_center.distSq(detections[i].center());

      if (min_dist > cur_dist)
        min_idx = i, min_dist = cur_dist;
    }

    if (min_idx != -1)
      face_frame.stablize(detections[min_idx], temporal_alpha, jerk_tolerance);

    return face_frame;
  }

  int getDetections()
  {
    int num_detects = 0;
    for (size_t i = 0; i < priors.size(); i++)
    {
      const float score = scores_tensor[2 * i + 1];
      if (score < score_threshold)
        continue;

      const Prior& p = priors[i];

      const float cx = boxes_tensor[4*i+0] * center_variance * p.w + p.cx;
      const float cy = boxes_tensor[4*i+1] * center_variance * p.h + p.cy;
      const float w  = expf(boxes_tensor[4*i+2] * size_variance) * p.w;
      const float h  = expf(boxes_tensor[4*i+3] * size_variance) * p.h;

      const float x1 = (cx - w * 0.5f) * ModelWidth();
      const float y1 = (cy - h * 0.5f) * ModelHeight();
      const float x2 = (cx + w * 0.5f) * ModelWidth();
      const float y2 = (cy + h * 0.5f) * ModelHeight();

      detections[num_detects++] = {x1, y1, x2, y2, score};
    }
    NonMaxSuppression(num_detects);

    return num_detects;
  }

  void NonMaxSuppression(int n)
  {
    if (n > top_k)
    {
      std::nth_element(
          detections.begin(),
          detections.begin() + top_k,
          detections.begin() + n,
          [](const Detection& a, const Detection& b) {
            return a.score > b.score;
          }
      );
      n = top_k;
    }
    std::sort(detections.begin(), detections.begin() + n,
        [](const Detection& a, const Detection& b) {
          return a.score > b.score;
        }
    );

    std::fill(selections.begin(), selections.begin()+n, true);
    for (int i = 0; i < n; i++)
    {
      if (!selections[i])
        continue;

      for (int j = i + 1; j < n; j++)
        if (selections[j] && detections[i].iou(detections[j]) > iou_threshold)
          selections[j] = false;
    }
  }

  void generate_priors()
  {
    const float invW = 1.f / ModelWidth();
    const float invH = 1.f / ModelHeight();

    for (int s = 0; s < num_boxes; s++)
    {
      const float stride = strides[s];
      const int fm_w = static_cast<int>(ceilf(ModelWidth() / stride));
      const int fm_h = static_cast<int>(ceilf(ModelHeight() / stride));

      const float cx_scale = stride * invW;
      const float cy_scale = stride * invH;
      for (int y = 0; y < fm_h; y++)
      {
        const float cy = (y + 0.5f) * cy_scale;
        for (int x = 0; x < fm_w; x++)
        {
          const float cx = (x + 0.5f) * cx_scale;
          for (int b = 0; b < box_counts[s]; b++)
          {
            const float box = min_boxes[s][b];
            priors.push_back({cx, cy, box * invW, box * invH});
          }
        }
      }
    }
  }

  int ModelWidth() const { return model.ModelWidth(); }
  int ModelHeight() const { return model.ModelHeight(); }
  int ModelPixels() const { return model.ModelWidth() * model.ModelHeight(); }

 private:

  Model<ModelType> model;

  ModelType* scores_tensor;

  ModelType* boxes_tensor;

  std::vector<Prior> priors;

  std::vector<bool> selections;

  std::vector<Detection> detections;

  Detection face_frame;

  const int top_k;

  const float score_threshold;

  const float iou_threshold;

  const float temporal_alpha;

  const float jerk_tolerance;
};

} // namespace spotlight

#endif // FACE_HPP
