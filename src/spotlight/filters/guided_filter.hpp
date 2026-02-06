/**
 * @file guided_filter.hpp
 * @author Ranjodh Singh
 *
 * @brief GUIDED_FILTER.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef GUIDED_FILTER_HPP
#define GUIDED_FILTER_HPP

#include <cmath>
#include <vector>
#include <algorithm>

#include <spotlight/utils/image_utils.hpp>
#include <spotlight/filters/box_filter.hpp>


namespace spotlight {

class GuidedFilter
{
 public:
  GuidedFilter(
    const float radius,
    const float eps,
    const int width,
    const int height,
    const int channels
  )
    : radius(radius), eps(eps), width(width), height(height),
      channels(channels), box_filter(radius, width, height, channels)
  {
    meanI.resize(height * width * channels);
    meanP.resize(height * width * channels);
    corrI.resize(height * width * channels);
    corrIp.resize(height * width * channels);
    A.resize(height * width * channels);
    B.resize(height * width * channels);
    meanA.resize(height * width * channels);
    meanB.resize(height * width * channels);
  }

  template <typename iT, typename gT, typename oT>
  void invoke(
    const iT* I,
    const gT* P,
    oT* Q,
    float clamp_lo = 0.0f,
    float clamp_hi = 1.0f
  )
  {
    box_filter.invoke(I, meanI.data());
    box_filter.invoke(P, meanP.data());

    box_filter.invoke(
      [&](int idx) {return (float)I[idx] * I[idx];},
      [&](int idx, float val) { corrI[idx] = val;}
    );
    box_filter.invoke(
      [&](int idx) {return (float)I[idx] * P[idx];},
      [&](int idx, float val) { corrIp[idx] = val;}
    );

    for (int i = 0; i < height * width * channels; i++)
    {
      A[i] = (corrIp[i] - meanI[i] * meanP[i]) /
             ((corrI[i] - meanI[i] * meanI[i]) + eps);
      B[i] = meanP[i] - A[i] * meanI[i];
    }

    box_filter.invoke<float, float>(A.data(), meanA.data());
    box_filter.invoke<float, float>(B.data(), meanB.data());

    // TODO: Does this need a clamp?
    for (int i = 0; i < height * width * channels; i++)
    {
      Q[i] = (oT)std::clamp(
        meanA[i] * I[i] + meanB[i], clamp_lo, clamp_hi
      );
    }
  }


  // TODO: FLOAT TAKES 4 BYTES :(
  // A MAJOR MAJOR MEMORY CONSUMER
  std::vector<float> meanI;
  std::vector<float> meanP;
  std::vector<float> corrI;
  std::vector<float> corrIp;
  std::vector<float> A;
  std::vector<float> B;
  std::vector<float> meanA;
  std::vector<float> meanB;

  const float radius;
  const float eps;
  const int width;
  const int height;
  const int channels;
  BoxFilter box_filter;
};

} // namespace spotlight

#endif // GUIDED_FILTER_HPP
