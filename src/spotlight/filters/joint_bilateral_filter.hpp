/**
 * @file joint_bilateral_filter.hpp
 * @author Ranjodh Singh
 *
 * @brief JOINT_BILATERAL_FILTER.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef JOINT_BILATERAL_FILTER_HPP
#define JOINT_BILATERAL_FILTER_HPP

#include <cmath>
#include <vector>
#include <algorithm>


namespace spotlight {

class JointBilateralFilter
{
 public:
  JointBilateralFilter(
    const float sigma_s,
    const float sigma_r,
    const int width,
    const int height,
    const int channels
  )
    : sigma_s(sigma_s), sigma_r(sigma_r), 
      width(width), height(height), channels(channels)
  {
    krad_s = (int)ceilf(3 * sigma_s);
    ksize_s = 2 * krad_s + 1;
    kernel_s.resize(ksize_s * ksize_s);
    scale_s = 1.0 / (2.0 * sigma_s * sigma_s);
    int k_idx = 0;
    for (int i = -krad_s; i <= krad_s; i++)
      for (int j = -krad_s; j <= krad_s; j++)
        kernel_s[k_idx++] = expf(-(i*i + j*j) * scale_s);

    // Who is HDR? I don't know him!
    kernel_r.resize(256);
    scale_r = 1.0 / (2.0 * sigma_r * sigma_r);
    for (int i = 0; i < 256; i++)
      kernel_r[i] = expf(-(i*i) * scale_r);
  }

  /* inp_g, inp_s and out need to have same no of channels */
  template <typename iT, typename gT, typename oT>
  void invoke(
    const iT* inp_s,
    const gT* inp_g,
    oT* out
  )
  {
    int idx_o = 0, idx_g = 0;
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        for (int c = 0; c < channels; c++)
        {
          int k_idx = 0;
          double nom = 0.0, denom = 0.0;
          for (int yk = -krad_s; yk <= krad_s; yk++)
          {
            for (int xk = -krad_s; xk <= krad_s; xk++)
            {
              const int yi = reflect(y + yk, height);
              const int xi = reflect(x + xk, width);
              const int inp_n = (yi * width + xi) * channels + c;

              const float g_r = kernel_r[
                std::clamp(
                  (int)fabsf((float)inp_g[idx_g] - (float)inp_g[inp_n]), 0, 255
                )
              ];
              const float g_s = kernel_s[k_idx++];

              nom += g_r * g_s * inp_s[inp_n];
              denom += g_r * g_s;
            }
          }
          // TODO: DIV BY ZERO (EPS)
          out[idx_o] = (oT)(nom / denom);
          idx_o++; idx_g++;
        }
      }
    }
  }

  inline int reflect(const int i, const int lim)
  {
    return i < 0 ? -i - 1 : (i >= lim ? 2 * lim - i - 1 : i);
  }


  int krad_s;
  int ksize_s;
  float scale_r;
  std::vector<float> kernel_r;
  float scale_s;
  std::vector<float> kernel_s;

  const float sigma_s;
  const float sigma_r;
  const int width;
  const int height;
  const int channels;
};

} // namespace spotlight

#endif // JOINT_BILATERAL_FILTER_HPP
