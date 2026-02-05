/**
 * @file laplacian_filter.hpp
 * @author Ranjodh Singh
 *
 * @brief LAPLACIAN_FILTER.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef LAPLACIAN_FILTER_HPP
#define LAPLACIAN_FILTER_HPP

#include <cmath>
#include <algorithm>


namespace spotlight {

class LaplacianFilter
{
 public:
  LaplacianFilter(
    const int /* radius */,
    const int width,
    const int height,
    const int channels
  )
    : width(width), height(height), channels(channels)
  {
    /* Nothing To Do */
  }

  template<typename iT, typename oT>
  void invoke(
    const iT* input,
    oT* output,
    const double clamp_lo = 0.f,
    const double clamp_hi = 1.f
  )
  {
    int idx = 0;
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        for (int c = 0; c < channels; c++)
        {
          int k_idx = 0;
          double sum = 0.0;
          for (int yk = -radius; yk <= radius; yk++)
          {
            for (int xk = -radius; xk <= radius; xk++)
            {
              const int yi = reflect(y + yk, height);
              const int xi = reflect(x + xk, width);
              const int cur_idx = (yi * width + xi) * channels + c;

               sum += kernel[k_idx++] * input[cur_idx];
            }
          }
          output[idx++] = (oT)std::clamp(sum, clamp_lo, clamp_hi);
        }
      }
    }
  }

  inline int reflect(const int i, const int lim)
  {
    return i < 0 ? -i - 1 : (i >= lim ? 2 * lim - i - 1 : i);
  }

  
  const int width;
  const int height;
  const int channels;

  static constexpr int radius = 1;
  static constexpr int kernel_size = 2 * radius + 1;
  static constexpr int kernel[] = {
    -1, -1, -1,
    -1, +8, -1,
    -1, -1, -1
  };
};

} // namespace spotlight

#endif // LAPLACIAN_FILTER_HPP
