/**
 * @file jpeg.hpp
 * @author Ranjodh Singh
 *
 * @brief JPEG.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef JPEG_HPP
#define JPEG_HPP

#include <cstring>
#include <turbojpeg.h>

#include <spotlight/utils/error_utils.hpp>
#include <spotlight/formats/converter.hpp>


namespace spotlight {

class ConverterJPEG final : public Converter
{
public:
  ConverterJPEG(int width, int height, int quality)
    : width(width),
      height(height),
      quality(quality),
      rgb_stride(3 * width)
  {
    compress_handle = tjInitCompress();
    if (!compress_handle)
      throw_err("tjInitCompress failed!");

    decompress_handle = tjInitDecompress();
    if (!decompress_handle)
    {
      tjDestroy(compress_handle);
      throw_err("tjInitDecompress failed!");
    }
  }

  ~ConverterJPEG() override
  {
    if (jpeg_buf)
      tjFree(jpeg_buf);
    if (compress_handle)
      tjDestroy(compress_handle);
    if (decompress_handle)
      tjDestroy(decompress_handle);
  }

  void decode(const uint8_t* jpeg, uint8_t* rgb, size_t size) override
  {
    int jpeg_width = 0, jpeg_height = 0;
    int jpeg_subsamp = 0, jpeg_colorspace = 0;

    if (
      tjDecompressHeader3(
        decompress_handle,
        jpeg,
        size,
        &jpeg_width,
        &jpeg_height,
        &jpeg_subsamp,
        &jpeg_colorspace
      ) != 0
    )
      throw_err(tjGetErrorStr2(decompress_handle));

    if (
      tjDecompress2(
        decompress_handle,
        jpeg,
        size,
        rgb,
        width,
        rgb_stride,
        height,
        TJPF_RGB,
        TJFLAG_FASTDCT
      ) != 0
    )
      throw_err(tjGetErrorStr2(decompress_handle));
  }

  void encode(const uint8_t* rgb, uint8_t* jpeg, size_t* size) override
  {
    const int format = TJPF_RGB;
    const int subsamp = TJSAMP_420;
    const int flags = TJFLAG_FASTDCT | TJFLAG_FASTUPSAMPLE;

    if (
      tjCompress2(
        compress_handle,
        rgb,
        width,
        rgb_stride,
        height,
        format,
        &jpeg_buf,
        &jpeg_size,
        subsamp,
        quality,
        flags
      ) != 0
    )
      throw_err(tjGetErrorStr2(compress_handle));

    if (jpeg_size <= *size)
    {
      *size = static_cast<size_t>(jpeg_size);
      std::memcpy(jpeg, jpeg_buf, jpeg_size);
    }
    else
    {
      throw_err(tjGetErrorStr2(compress_handle));
    }
  }

private:
  tjhandle compress_handle = nullptr;
  tjhandle decompress_handle = nullptr;

  unsigned char* jpeg_buf = nullptr;
  unsigned long jpeg_size = 0;

  const int width;
  const int height;
  const int quality;
  const int rgb_stride;
};

} // namespace spotlight

#endif // JPEG_HPP
