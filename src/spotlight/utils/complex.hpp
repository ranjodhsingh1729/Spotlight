/**
 * @file complex.hpp
 * @author Ranjodh Singh
 *
 * @brief COMPLEX.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef COMPLEX_HPP
#define COMPLEX_HPP

#include <cmath>


namespace spotlight {

struct Complex {
  float re, im;

  inline float mod() const noexcept {
    return sqrt(re * re + im * im);
  }

  inline Complex operator+(const Complex& b) const noexcept {
    return {re + b.re, im + b.im};
  }

  inline Complex& operator+=(const Complex& b) noexcept {
    re += b.re;
    im += b.im;
    return *this;
  }

  inline Complex operator*(const Complex& b) const noexcept {
    return {
      re * b.re - im * b.im,
      re * b.im + im * b.re
    };
  }

  inline Complex& operator*=(const Complex& b) noexcept {
    const float r = re * b.re - im * b.im;
    const float i = re * b.im + im * b.re;
    re = r;
    im = i;
    return *this;
  }

  inline Complex operator/(const Complex& b) const noexcept {
    return {
      (re*b.re + im*b.im) / (b.re*b.re + b.im*b.im),
      (im*b.re - re*b.im) / (b.re*b.re + b.im*b.im),
    };
  }

  inline Complex& operator/=(const Complex& b) noexcept {
    const float r = (re*b.re + im*b.im) / (b.re*b.re + b.im*b.im);
    const float i = (im*b.re - re*b.im) / (b.re*b.re + b.im*b.im);
    re = r;
    im = i;
    return *this;
  }

  inline Complex operator+(const float x) const noexcept {
    return { re + x, im + x };
  }

  inline Complex& operator+=(const float x) noexcept {
    re += x; im += x;
    return *this;
  }
  
  inline Complex operator*(const float x) const noexcept {
    return { re * x, im * x };
  }

  inline Complex& operator*=(const float x) noexcept {
    re *= x; im *= x;
    return *this;
  }

  inline Complex operator/(const float x) const noexcept {
    return { re / x, im / x };
  }

  inline Complex& operator/=(const float x) noexcept {
    re /= x; im /= x;
    return *this;
  }
};

} // namespace spotlight

#endif // COMPLEX_HPP
