/**
 * @file lens_filter.hpp
 * @author Ranjodh Singh
 *
 * @brief LENS_FILTER. (https://dl.acm.org/doi/10.1145/3084363.3085022)
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef LENS_FILTER_HPP
#define LENS_FILTER_HPP

#include <vector>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include <spotlight/utils/complex.hpp>


namespace spotlight {

class LensFilter
{
 public:
  struct KernelParam
  {
    float a, b, A, B;
  };

  LensFilter(
    const int radius,
    const int components,
    const float transition,
    const int width,
    const int height,
    const int channels
  )
    : radius(radius), components(components), transition(transition),
      width(width), height(height), channels(channels)
  {
    kernel_size = 2 * radius + 1;
    param_offset = components * (components - 1) / 2;

    kernels.resize(kernel_size * components);
    tmp.resize(height * width * channels * components);
    acc.resize(components);

    generateNormalizedKernels();
  }

  Complex kernelFunction(const float i, const KernelParam& p)
  {
    return {
      expf(-p.a * i * i) * cosf(p.b * i * i),
      expf(-p.a * i * i) * sinf(p.b * i * i),
    };
  };

  void generateNormalizedKernels()
  {
    int k_idx = 0;
    const float scale = (1.f + transition) / radius;
    for (int i = -radius; i <= radius; i++)
    {
      for (int c = 0; c < components; c++)
      {
        kernels[k_idx++] = kernelFunction(
          scale * i, KernelParams[param_offset+c]
        );
      }
    }

    double sum = 0.0;
    for (int i = 0; i < kernel_size; i++)
    {
      for (int j = 0; j < kernel_size; j++)
      {
        for (int c = 0; c < components; c++)
        {
          const Complex product = (
            kernels[i*components+c] * kernels[j*components+c]
          );
          sum += (
            KernelParams[param_offset+c].A * product.re
            +
            KernelParams[param_offset+c].B * product.im
          );
        }
      }
    }

    k_idx = 0;
    const double norm = 1.0 / sqrt(sum);
    for (int i = 0; i < kernel_size; i++)
    {
      for (int c = 0; c < components; c++)
      {
        kernels[k_idx++] *= norm;
      }
    }

    // printNormalizedKernels();
  }

  void printNormalizedKernels()
  {
    std::cout << std::left
              << std::setw(25) << "re"
              << std::setw(25) << "im"
              << std::setw(25) << "A"
              << std::setw(25) << "B"
              << '\n';
    std::cout << std::string(100, '-') << '\n';
    std::cout << std::setprecision(20) << std::fixed;

    int k_idx = 0;
    for (int i = 0; i < kernel_size; i++)
    {
      for (int c = 0; c < components; c++)
      {
        std::cout << std::setw(25) << kernels[k_idx].re
                  << std::setw(25) << kernels[k_idx].im
                  << std::setw(25) << KernelParams[param_offset+c].A 
                  << std::setw(25) << KernelParams[param_offset+c].B
                  << '\n';

        k_idx++;
      }
      std::cout << '\n';
    }
    std::flush(std::cout);
  }

  template <typename iT, typename oT>
  void invoke(const iT* input, oT* output, const float* mask)
  {
    horizontal_pass(input, mask);
    vertical_pass(output, mask);
  }

  template <typename T>
  void horizontal_pass(const T* input, const float* mask)
  {
    int idx = 0;
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        for (int c = 0; c < channels; c++)
        {
          const int idxc = y * width + x;
          for (int i = 0; i < components; i++)
            acc[i] = {0, 0};

          int k_idx = 0;
          for (int i = -radius; i <= radius; i++)
          { 
            const int sx = reflect(x + i, width);

            const int idxn = y * width + sx;
            const int idxc_c = idxc * channels + c;
            const int idxn_c = idxn * channels + c;
            const bool mask_val = mask[idxn] > 0.5;
            const int src_idx = mask_val * idxc_c + !mask_val * idxn_c;

            for (int k = 0; k < components; k++)
              acc[k] += kernels[k_idx++] * input[src_idx];
          }

          for (int k = 0; k < components; k++)
            tmp[idx++] = acc[k];
        }
      }
    }
  }

  template <typename T>
  void vertical_pass(T* output, const float* mask)
  {
    int idx = 0;
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        const int idxc = y * width + x;
        for (int c = 0; c < channels; c++)
        {
          for (int i = 0; i < components; i++)
            acc[i] = {0, 0};

          int k_idx = 0;
          for (int i = -radius; i <= radius; i++)
          {
            const int sy = reflect(y + i, height);

            const int idxn = sy * width + x;
            const int idxc_c = idxc * channels + c;
            const int idxn_c = idxn * channels + c;
            const bool mask_val = mask[idxn] > 0.5;
            const int buf_idx = mask_val * idxc_c + !mask_val * idxn_c;

            const Complex* src = &tmp[buf_idx * components];
            for (int k = 0; k < components; k++)
              acc[k] += kernels[k_idx++] * src[k];
          }

          double sum = 0.0;
          for (int k = 0; k < components; k++)
          {
            sum += (
              KernelParams[param_offset+k].A * acc[k].re
              +
              KernelParams[param_offset+k].B * acc[k].im
            );

          }
          output[idx++] = (T)std::clamp(sum, 0.0, 255.0);
        }
      }
    }
  }

  inline int reflect(const int i, const int lim)
  {
    return i < 0 ? -i - 1 : (i >= lim ? 2 * lim - i - 1 : i);
  }


  const int radius;
  const int components;
  const float transition;
  const int width;
  const int height;
  const int channels;

  int param_offset;
  int kernel_size;
  std::vector<Complex> tmp;
  std::vector<Complex> acc;
  std::vector<Complex> kernels;

  static constexpr KernelParam KernelParams[] = {
    { 0.862325f, 1.624835f, 0.767583f, 1.862321f },

    { 0.886528f, 5.268909f,  0.411259f, -0.548794f },
    { 1.960518f, 1.558213f,  0.513282f,  4.56111f  },

    { 2.17649f,  5.043495f,  1.621035f, -2.105439f },
    { 1.019306f, 9.027613f, -0.28086f,  -0.162882f },
    { 2.81511f,  1.597273f, -0.366471f, 10.300301f },

    { 4.338459f,  1.553635f, -5.767909f,  46.164397f },
    { 3.839993f,  4.693183f,  9.795391f, -15.227561f },
    { 2.79188f,   8.178137f, -3.048324f,   0.302959f },
    { 1.34219f,  12.328289f,  0.010001f,   0.24465f  },

    { 4.892608f,  1.685979f, -22.356787f,  85.91246f  },
    { 4.71187f,   4.998496f,  35.918936f, -28.875618f },
    { 4.052795f,  8.244168f, -13.212253f,  -1.578428f },
    { 2.929212f, 11.900859f,   0.507991f,   1.816328f },
    { 1.512961f, 16.116382f,   0.138051f,  -0.01f     },

    { 5.143778f,  2.079813f, -82.326596f, 111.231024f },
    { 5.612426f,  6.153387f, 113.878661f,  58.004879f },
    { 5.982921f,  9.802895f,  39.479083f,-162.028887f },
    { 6.505167f, 11.059237f, -71.286026f,  95.027069f },
    { 3.869579f, 14.81052f,    1.405746f,  -3.704914f },
    { 2.201904f, 19.032909f,  -0.152784f,  -0.107988f }
  };
};

} // namespace spotlight

#endif // LENS_FILTER_HPP
