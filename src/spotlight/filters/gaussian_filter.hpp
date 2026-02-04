/**
 * @file gaussian_filter.hpp
 * @author Ranjodh Singh
 *
 * @brief GAUSSIAN_FILTER.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef GAUSSIAN_FILTER_HPP
#define GAUSSIAN_FILTER_HPP

#include <cmath>
#include <vector>


namespace spotlight {

class GaussianFilter
{
 public:
  GaussianFilter(
    const int radius,
    const int width,
    const int height,
    const int channels
  )
    : radius(radius), width(width), height(height), channels(channels)
  {
    sigma = radius / 3.f;
    kernel_size = 2 * radius + 1;
    kernel.resize(kernel_size);
    const double kernel_scale = 1.0 / (2.0 * sigma * sigma);

    double sum = 0.0;
    for (int i = -radius; i <= radius; i++)
    {
      const float value = expf(-(i * i) * kernel_scale);
      kernel[i + radius] = value;
      sum += value;
    }

    for (auto &i: kernel)
      i /= sum;

    buffer.resize(height * width * channels);
  }

  template <typename iT, typename oT>
  void invoke(
    const iT* inp,
    oT* out
  )
  {
    int idx = 0;
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        for (int c = 0; c < channels; c++)
        {
          int idx_k = 0;
          double sum = 0.0;
          for (int i = -radius; i <= radius; i++)
          {
            const int sy = reflect(y + i, height);
            sum += kernel[idx_k++] * inp[(sy * width + x) * channels + c];
          }
          buffer[idx++] = (float)sum;
        }
      }
    }

    idx = 0;
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        for (int c = 0; c < channels; c++)
        {
          int idx_k = 0;
          double sum = 0.0;
          for (int i = -radius; i <= radius; i++)
          {
            const int sx = reflect(x + i, width);
            sum += kernel[idx_k++] * buffer[(y * width + sx) * channels + c];
          }
          out[idx++] = (oT)sum;
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
  std::vector<float> buffer;

  const int radius;
  const int width;
  const int height;
  const int channels;
};

} // namespace spotlight

#endif // GAUSSIAN_FILTER_HPP
