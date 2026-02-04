/**
 * @file converter.hpp
 * @author Ranjodh Singh
 *
 * @brief CONVERTER.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <cstdint>
#include <stddef.h>


namespace spotlight {

class Converter
{
 public:
  virtual ~Converter() = default;
  virtual void decode(const uint8_t* src, uint8_t* dest, size_t size) = 0;
  virtual void encode(const uint8_t* src, uint8_t* dest, size_t* size) = 0;
};

} // namespace spotlight

#endif // CONVERTER_HPP
