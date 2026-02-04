/**
 * @file box_filter.hpp
 * @author Ranjodh Singh
 *
 * @brief BOX_FILTER.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef BOX_FILTER_HPP
#define BOX_FILTER_HPP


namespace spotlight {

class BoxFilter
{
 public:
  BoxFilter(
    const int radius,
    const int width,
    const int height,
    const int channels
  )
    : radius(radius), width(width), height(height), channels(channels)
  {
    kernel_size = 2 * radius + 1;
    kernel_value = 1.0 / (kernel_size * kernel_size);
  }

  template <typename iT, typename oT>
  void invoke(
    const iT* input,
    oT* output
  )
  {
    invoke(
      [&](int idx) {return (iT)input[idx];},
      [&](int idx, oT val) {output[idx] = val;}
    );
  }

  template <typename iF, typename oF>
  void invoke(
    const iF inp_func,
    oF out_func
  )
  {
    int idx = 0;
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        for (int c = 0; c < channels; c++)
        {
          double sum = 0.0;
          for (int yk = -radius; yk <= radius; yk++)
          {
            for (int xk = -radius; xk <= radius; xk++)
            {
              const int yi = reflect(y + yk, height);
              const int xi = reflect(x + xk, width);
              const int cur_idx = (yi * width + xi) * channels + c;

              // iT to double
              sum += inp_func(cur_idx);
            }
          }
          // double to oT
          out_func(idx++, kernel_value * sum);
        }
      }
    }
  }

  inline int reflect(const int i, const int lim)
  {
    return i < 0 ? -i - 1 : (i >= lim ? 2 * lim - i - 1 : i);
  }

  inline int reflect_branchless(const int i, const int lim)
  {
    return (
      (i < 0) *
        (-i - 1)
      +
      !(i < 0) *
        (
          (i >= lim) *
            (2 * lim - i - 1)
          +
          !(i >= lim) *
            i
        )
      );
  }


  int kernel_size;
  double kernel_value;

  const int radius;
  const int width;
  const int height;
  const int channels;
};

} // namespace spotlight

#endif // BOX_FILTER_HPP
