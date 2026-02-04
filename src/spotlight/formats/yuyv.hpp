/**
 * @file yuyv.hpp
 * @author Ranjodh Singh
 *
 * @brief YUYV.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef YUYV_HPP
#define YUYV_HPP

#include <vector>
#include <cstdint>
#include <libyuv.h>

#include <spotlight/utils/error_utils.hpp>
#include <spotlight/formats/converter.hpp>


namespace spotlight {

class ConverterYUYV final : public Converter
{
public:
  ConverterYUYV(int width, int height, int /* quality */)
    : width(width),
      height(height),
      yuyv_stride(2 * width),
      rgb_stride(3 * width),
      argb_stride(4 * width)
  {
    vec_argb_buffer.resize(4 * width * height);
    argb_buffer = vec_argb_buffer.data();
  }

  void decode(const uint8_t* yuyv, uint8_t* rgb, size_t /* size */) override
  {
    if (
      libyuv::YUY2ToARGB(
        yuyv,
        yuyv_stride,
        argb_buffer,
        argb_stride,
        width,
        height
      ) != 0
    )
      throw_err("YUY2ToARGB failed!");

    if (
      libyuv::ARGBToRGB24(
        argb_buffer,
        argb_stride,
        rgb,
        rgb_stride,
        width,
        height
      ) != 0
    )
      throw_err("ARGBToRGB24 failed!");
  }

  void encode(const uint8_t* rgb, uint8_t* yuyv, size_t* /* size */) override
  {
    if (
      libyuv::RGB24ToARGB(
        rgb,
        rgb_stride,
        argb_buffer,
        argb_stride,
        width,
        height
      ) != 0
    )
      throw_err("RGB24ToARGB failed!");

    if (
      libyuv::ARGBToYUY2(
        argb_buffer,
        argb_stride,
        yuyv,
        yuyv_stride,
        width,
        height
      ) != 0
    )
      throw_err("ARGBToYUY2 failed!");
  }

private:
  std::vector<uint8_t> vec_argb_buffer;
  uint8_t* argb_buffer;

  const int width;
  const int height;
  const int yuyv_stride;
  const int rgb_stride;
  const int argb_stride;
};

} // namespace spotlight

#endif // YUYV_HPP
