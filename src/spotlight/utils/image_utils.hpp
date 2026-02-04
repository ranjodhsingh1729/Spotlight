/**
 * @file image_utils.hpp
 * @author Ranjodh Singh
 *
 * @brief IMAGE_UTILS.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILS_HPP

#include <cmath>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <spotlight/utils/error_utils.hpp>


namespace spotlight {

template <typename T>
inline T clamp_branchless(
  const T x, const T lo, const T hi
)
{
  return (
    (x < lo)
      * lo
    +
    !(x < lo)
      * (
        (x < hi)
          * hi
        +
        !(x < hi)
          * x
      )
  );
}

template <typename T>
inline size_t frame_size(
  const int width,
  const int height,
  const int channels
)
{
  return width * height * channels * sizeof(T);
}

inline void convert_u8_to_f32(
  const uint8_t* in,
  float* out,
  const int width,
  const int height,
  const int channels
)
{
  const int n = channels * width * height;

  for (int i = 0; i < n; i++)
    out[i] = (float)in[i];
}

inline void convert_f32_to_u8(
  const float* in,
  uint8_t* out,
  const int width,
  const int height,
  const int channels
)
{
  const int n = channels * width * height;

  static constexpr uint8_t lo = 0;
  static constexpr uint8_t hi = 255;

  for (int i = 0; i < n; i++)
    out[i] = std::clamp((uint8_t)(in[i]+0.5f), lo, hi);
}

template <typename iT, typename oT>
inline void scale(
  const iT* in,
  oT* out,
  const int width,
  const int height,
  const int channels,
  const float alpha = 1.0,
  const float beta = 0.0
)
{
  const int n = channels * width * height;

  for (int i = 0; i < n; i++)
    out[i] = (oT)(in[i] * alpha + beta);
}

template<typename iT, typename oT>
inline void rgb2gray(
  const iT* inp,
  oT* out,
  const int width,
  const int height
)
{
  const int pixels = width * height;

  for (int i = 0; i < pixels; i++)
      out[i] = (oT)(0.299 * inp[3*i] + 0.587 * inp[3*i+1] + 0.114 * inp[3*i+2]);
}

template<typename iT, typename oT>
inline void gray2rgb(
  const iT* inp,
  oT* out,
  const int width,
  const int height
)
{
  const int pixels = width * height;

  for (int i = 0; i < pixels; i++)
      out[3*i] = out[3*i+1] = out[3*i+2] = (oT)inp[i];
}

template <typename fgT, typename bgT, typename oT>
inline void alpha_blend(
  const fgT* fg,
  const bgT* bg,
  oT* output,
  const float* mask,
  const int width,
  const int height,
  const int channels
)
{
  const int pixels = width * height;

  oT *res = output;
  const fgT *fgp = fg;
  const bgT *bgp = bg;
  for (int i = 0; i < pixels; i++)
  {
    const float m_alpha = mask[i];
    const float m_beta = 1.f - m_alpha;

    for (int c = 0; c < channels; c++)
      *(res++) = (oT)(m_alpha * *(fgp++) + m_beta * *(bgp++));
  }
}

template <typename fgT, typename bgT, typename oT>
inline void light_wrap(
  const fgT* fg,
  const bgT* bg,
  oT* output,
  const float* edge,
  const float* mask,
  const int width,
  const int height,
  const int channels
)
{
  const int pixels = width * height;

  oT *res = output;
  const fgT *fgp = fg;
  const bgT *bgp = bg;
  for (int i = 0; i < pixels; i++)
  {
    const float m_alpha = mask[i];
    const float m_beta = 1.f - m_alpha;
    const float e_alpha = edge[i];
    const float e_beta = 1.f - e_alpha;

    for (int c = 0; c < channels; c++)
    {
      *(res++) = (oT)(
        e_beta * (m_alpha * (*fgp) + m_beta * (*bgp)) + e_alpha * (*bgp)
      );
      fgp++, bgp++;
    }
  }
}

template <typename iT, typename oT>
inline void resize_nn(
  const iT* inp,
  oT* out,
  const int inp_width,
  const int inp_height,
  const int out_width,
  const int out_height,
  const int channels
)
{
  oT* dst = out;
  const float scaleX = (float)inp_width  / out_width;
  const float scaleY = (float)inp_height / out_height;

  float ys = 0;
  for (int y = 0; y < out_height; y++)
  {
    const int yi = (int)ys;
    const iT* src_row = inp + yi * inp_width * channels;

    float xs = 0;
    for (int x = 0; x < out_width; x++)
    {
      const int xi = (int)xs;
      const iT* src = src_row + xi * channels;

      for (int c = 0; c < channels; c++)
        *(dst++) = (oT)src[c];

      xs += scaleX;
    }
    ys += scaleY;
  }
}

template <typename iT, typename oT>
inline void resize_bilinear(
  const iT* inp,
  oT* out,
  const int inp_width,
  const int inp_height,
  const int out_width,
  const int out_height,
  const int channels
)
{
  const float scaleX = (
    out_width > 1 ? (float)(inp_width - 1) / (out_width - 1) : 0.f
  );
  const float scaleY = (
    out_height > 1 ? (float)(inp_height - 1) / (out_height - 1) : 0.f
  );

  std::vector<int> X0(out_width);
  std::vector<int> X1(out_width);
  std::vector<float> XF(out_width);
  for (int x = 0; x < out_width; x++)
  {
      const float xs = x * scaleX;
      X0[x] = (int)floorf(xs);
      X1[x] = (int)ceilf(xs);
      XF[x] = xs - X0[x];
  }

  oT* dst = out;
  for (int y = 0; y < out_height; y++)
  {
    const float ys = y * scaleY;
    const int y0 = (int)floorf(ys);
    const int y1 = (int)ceilf(ys);
    const float yf = ys - y0;

    const iT* col0 = inp + y0 * inp_width * channels;
    const iT* col1 = inp + y1 * inp_width * channels;
    for (int x = 0; x < out_width; x++)
    {
      const int x0 = X0[x];
      const int x1 = X1[x];
      const float xf = XF[x];

      const iT* p00 = col0 + x0 * channels;
      const iT* p10 = col0 + x1 * channels;
      const iT* p01 = col1 + x0 * channels;
      const iT* p11 = col1 + x1 * channels;
      for (int c = 0; c < channels; c++)
      {
        const float i0 = p00[c] + (p10[c] - p00[c]) * xf;
        const float i1 = p01[c] + (p11[c] - p01[c]) * xf;

        *(dst++) = (oT)(i0 + (i1 - i0) * yf);
      }
    }
  }
}

} // namespace spotlight

#endif // IMAGE_UTILS_HPP
