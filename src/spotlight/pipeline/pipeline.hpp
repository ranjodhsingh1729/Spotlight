/**
 * @file pipeline.hpp
 * @author Ranjodh Singh
 *
 * @brief PIPELINE.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <cstdint>
#include <spotlight/config/config.hpp>
#include <spotlight/config/defaults.hpp>
#include <spotlight/models/segm/segm.hpp>
#include <spotlight/utils/load_png.hpp>
#include <spotlight/utils/image_utils.hpp>
#include <spotlight/filters/log_filter.hpp>
#include <spotlight/filters/lens_filter.hpp>
#include <spotlight/filters/guided_filter.hpp>
#include <spotlight/filters/gaussian_filter.hpp>
#include <spotlight/filters/laplacian_filter.hpp>
#include <spotlight/filters/joint_bilateral_filter.hpp>


namespace spotlight {

class Pipeline
{
 public:
  Pipeline(const PipelineConfig& cfg)
    : cfg(cfg),
      segm(SEGM_MODEL_PATH, cfg.num_threads),
      mask_filter(
        MASK_FILTER_RADIUS,
        segm.ModelWidth(), segm.ModelHeight(), 1
      ),
      edge_filter(
        EDGE_FILTER_RADIUS,
        segm.ModelWidth(), segm.ModelHeight(), 1
      ),
      blur_filter(
        BLUR_FILTER_RADIUS,
        BLUR_FILTER_COMPONENTS,
        BLUR_FILTER_TRANSITION,
        segm.ModelWidth(), segm.ModelHeight(), 3
      )
  {
    vec_inp_segm.resize(3 * segm.ModelPixels());
    vec_out_segm.resize(1 * segm.ModelPixels());
    vec_mask_s.resize(1 * segm.ModelPixels());
    vec_mask_l.resize(1 * cfg.OutPixels());
    inp_segm = vec_inp_segm.data();
    out_segm = vec_out_segm.data();
    mask_s = vec_mask_s.data();
    mask_l = vec_mask_l.data();

    switch (cfg.pipeline_mode)
    {
      case PipelineMode::BLUR:
        vec_blur_s.resize(3 * segm.ModelPixels());
        vec_blur_l.resize(3 * cfg.OutPixels());
        blur_s = vec_blur_s.data();
        blur_l = vec_blur_l.data();
        break;
      case PipelineMode::IMAGE:
        vec_bg_img.resize(3 * cfg.OutPixels());
        bg_img = vec_bg_img.data();
        load_PNG(
          cfg.bg_img_path,
          bg_img, 3 * cfg.OutPixels() * sizeof(float),
          cfg.out_width, cfg.out_height, 3
        );
        break;
      case PipelineMode::VIDEO:
        throw_err("PipelineMode unsupported yet!!!");
        break;
      default:
        throw_err("Invalid PipelineMode!!!");
    }
  }


  void invoke(const uint8_t* inp_u, uint8_t* out_u)
  {
    spotlight::resize_bilinear(
      inp_u, inp_segm,
      cfg.inp_width, cfg.inp_height,
      segm.ModelWidth(), segm.ModelHeight(), 3
    );
    spotlight::scale(
      inp_segm, inp_segm,
      segm.ModelWidth(), segm.ModelHeight(), 3, 1.f / 255.f
    );
    segm.invoke(inp_segm, out_segm);
    spotlight::scale(
      inp_segm, inp_segm,
      segm.ModelWidth(), segm.ModelHeight(), 3, 255.f / 1.f
    );

    mask_filter.invoke(out_segm, mask_s);
    spotlight::resize_bilinear(
      mask_s, mask_l,
      segm.ModelWidth(), segm.ModelHeight(),
      cfg.out_width, cfg.out_height, 1
    );

    switch (cfg.pipeline_mode)
    {
      case PipelineMode::BLUR:
        blur_filter.invoke(inp_segm, blur_s, out_segm);
        spotlight::resize_bilinear(
          blur_s, blur_l,
          segm.ModelWidth(), segm.ModelHeight(),
          cfg.out_width, cfg.out_height, 3
        );
        spotlight::alpha_blend(
          inp_u, blur_l, out_u, mask_l,
          cfg.out_width, cfg.out_height, 3
        );
        break;
      case PipelineMode::IMAGE:
        spotlight::alpha_blend(
          inp_u, bg_img, out_u, mask_l,
          cfg.out_width, cfg.out_height, 3
        );
        break;
      case PipelineMode::VIDEO:
        throw_err("PipelineMode unsupported yet!!!");
        break;
      default:
        throw_err("Invalid PipelineMode!!!");
    }
  }


  const PipelineConfig& cfg;
  SelfieSegmentation<float> segm;

  GaussianFilter mask_filter;
  LaplacianFilter edge_filter;
  LensFilter blur_filter;

  std::vector<float> vec_inp_segm;
  std::vector<float> vec_out_segm;
  std::vector<float> vec_mask_s;
  std::vector<float> vec_mask_l;
  std::vector<uint8_t> vec_bg_img;
  std::vector<uint8_t> vec_blur_s;
  std::vector<uint8_t> vec_blur_l;

  float *inp_segm, *out_segm, *mask_s, *mask_l;
  uint8_t *bg_img, *blur_s, *blur_l;
};

} // namespace spotlight

#endif // PIPELINE_HPP
