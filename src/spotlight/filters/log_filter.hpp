/**
 * @file log_filter.hpp
 * @author Ranjodh Singh
 *
 * @brief LOG_FILTER.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef LOG_FILTER_HPP
#define LOG_FILTER_HPP

#include <cmath>
#include <vector>
#include <algorithm>


namespace spotlight {

class LOGFilter
{
 public:
  LOGFilter(
    const int radius, const int width, const int height, const int channels
  )
    : radius(radius), width(width), height(height), channels(channels)
  {
    // Note: sigma < 1 (radius < 3) can be unstable
    sigma = radius / 3.f;
    kernel_size = 2 * radius + 1;
    kernel.resize(kernel_size * kernel_size);

    const double pi = M_PI;
    const double sigmaSq = sigma * sigma;
    const double scale_a = 1.0 / (2  * sigmaSq);
    const double scale_b = 1.0 / (pi * sigmaSq * sigmaSq);

    int k_idx = 0;
    double sum = 0.0;
    for (int y = -radius; y <= radius; y++)
    {
      for (int x = -radius; x <= radius; x++)
      {
        const float tmp = (y*y + x*x) * scale_a;
        const float val = scale_b * (tmp - 1) * expf(-tmp);
        kernel[k_idx++] = val;
        sum += val;
      }
    }

    const float mean = sum / (kernel_size * kernel_size);
    for (auto &i: kernel)
        i -= mean;
  }

  template<typename iT, typename oT>
  void invoke(
    const iT* input,
    oT* output,
    const double clamp_lo = 0.0,
    const double clamp_hi = 1.0
  )
  {
    oT* dst = output;
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
          *(dst++) = (oT)std::clamp(sum, clamp_lo, clamp_hi);
        }
      }
    }
  }

  inline int reflect(const int i, const int lim)
  {
    return i < 0 ? -i - 1 : (i >= lim ? 2 * lim - i - 1 : i);
  }


  float sigma;
  int kernel_size;
  std::vector<float> kernel;
  
  const int radius;
  const int width;
  const int height;
  const int channels;
};

} // namespace spotlight

#endif // LOG_FILTER_HPP
