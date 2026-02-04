/**
 * @file load_png.hpp
 * @author Ranjodh Singh
 *
 * @brief LOAD_PNG.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef LOAD_PNG_HPP
#define LOAD_PNG_HPP

#include <spng.h>
#include <stdexcept>

#include <spotlight/utils/image_utils.hpp>

#define SPNG_CHUNK_LIMIT    64 * 1024 * 1024


namespace spotlight {

template <typename T>
void load_PNG(
  const std::string& path,
  T* buffer,
  const size_t buffer_size,
  const size_t width,
  const size_t height,
  const size_t channels
)
{
  uint8_t* image = nullptr;

  FILE* file = nullptr;
  file = fopen(path.c_str(), "rb");
  if (!file)
    throw std::runtime_error("Failed to open PNG!");

  spng_ctx* ctx = nullptr;
  ctx = spng_ctx_new(0);
  if (!ctx)
    throw std::runtime_error("Failed spng_ctx_new!");

  try
  {
    spng_set_crc_action(ctx, SPNG_CRC_ERROR, SPNG_CRC_ERROR);
    spng_set_chunk_limits(ctx, SPNG_CHUNK_LIMIT, SPNG_CHUNK_LIMIT);

    if (spng_set_png_file(ctx, file))
      throw std::runtime_error("Failed spng_set_png_file!");

    struct spng_ihdr ihdr;
    if (spng_get_ihdr(ctx, &ihdr))
      throw std::runtime_error("Failed spng_get_ihdr!");

    size_t size;
    if (spng_decoded_image_size(ctx, SPNG_FMT_RGB8, &size))
      throw std::runtime_error("Failed spng_decoded_image_size!");

    image = new uint8_t[size];
    if (!image)
      throw std::runtime_error("Failed to allocate buffer for PNG!");

    if (spng_decode_image(ctx, image, size, SPNG_FMT_RGB8, 0))
      throw std::runtime_error("Failed spng_decode_image!");

    resize_bilinear(
      image, buffer, ihdr.width, ihdr.height, width, height, channels
    );
  }
  catch (...)
  {
    fclose(file);
    spng_ctx_free(ctx);
    delete[] image;
    throw;
  }
  fclose(file);
  spng_ctx_free(ctx);
  delete[] image;
}

} // namespace spotlight

#endif // LOAD_PNG_HPP
