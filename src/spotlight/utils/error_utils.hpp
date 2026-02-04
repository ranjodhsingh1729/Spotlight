/**
 * @file error_utils.hpp
 * @author Ranjodh Singh
 *
 * @brief ERROR_UTILS.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef ERROR_UTILS_HPP
#define ERROR_UTILS_HPP

#include <string.h>
#include <iostream>
#include <stdexcept>


namespace spotlight {

[[noreturn]] inline void throw_err(const std::string_view msg)
{
  throw std::runtime_error(std::string(msg));
}

[[noreturn]] inline void throw_errno(const std::string_view msg)
{
  const int err = errno;
  throw std::runtime_error(
    std::string(strerror(err)) + ": " + std::string(msg)
  );
}

inline void log_err(const std::string_view msg) noexcept
{
  std::cerr << msg << std::endl;
}

inline void log_errno(const std::string_view msg) noexcept
{
  const int err = errno;
  std::cerr << strerror(err) << ": " << msg << std::endl;
}

} // namespace spotlight

#endif // ERROR_UTILS_HPP
