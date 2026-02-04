/**
 * @file face_utils.hpp
 * @author Ranjodh Singh
 *
 * @brief FACE_UTILS.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef FACE_UTILS_HPP
#define FACE_UTILS_HPP

#include <algorithm>


namespace spotlight {

struct Prior {
  float cx, cy, w, h;
};

struct Point
{
  float x, y;

  float distSq(const Point& other) const
  {
    const float dx = x - other.x;
    const float dy = y - other.y;
    return dx * dx + dy * dy;
  }
};

struct Detection
{
  float x1, y1, x2, y2, score;

  float width() const { return x2 - x1; }
  float height() const { return y2 - y1; }

  float area() const { return width() * height(); }
  Point center() const { return {(x1 + x2) * 0.5f, (y1 + y2) * 0.5f}; }

  float iou(const Detection& other) const
  {
    const float xx1 = std::max(x1, other.x1);
    const float yy1 = std::max(y1, other.y1);
    const float xx2 = std::min(x2, other.x2);
    const float yy2 = std::min(y2, other.y2);

    const float w = std::max(0.f, xx2 - xx1);
    const float h = std::max(0.f, yy2 - yy1);
    if (w <= 0.f || h <= 0.f)
      return 0.f;

    const float inter = w * h;
    return inter / (area() + other.area() - inter + 1e-6f);
  }

  void stablize(
    const Detection& d, const float alpha, const float tolerance
  )
  {
    const float dx = std::abs(x1 - d.x1) + std::abs(x2 - d.x2);
    const float dy = std::abs(y1 - d.y1) + std::abs(y2 - d.y2);

    const float motion = (dx / width()) + (dy / height());
    if (motion < tolerance)
      return;

    const float beta = 1.f - alpha;
    x1 = alpha * x1 + beta * d.x1;
    x2 = alpha * x2 + beta * d.x2;
    y1 = alpha * y1 + beta * d.y1;
    y2 = alpha * y2 + beta * d.y2;
  }

  void frame(const float aspect_ratio)
  {
    const Point c = center();
    const float w0 = width();
    const float h0 = height();
    const float w = std::max(w0, h0 * aspect_ratio);
    const float h = std::max(h0, w0 / aspect_ratio);

    x1 = c.x - 0.5f * w;
    y1 = c.y - 0.5f * h;
    x2 = c.x + 0.5f * w;
    y2 = c.y + 0.5f * h;
  }

  void pad(
    const float left,
    const float right,
    const float top,
    const float bottom
  )
  {
    const float w = width();
    const float h = height();

    x1 -= w * left;
    y1 -= h * top;
    x2 += w * right;
    y2 += h * bottom;
  }

  void scale(const float factorW, const float factorH)
  {
    x1 *= factorW;
    y1 *= factorH;
    x2 *= factorW;
    y2 *= factorH;
  }

  void clamp(const int width, const int height)
  {
    x1 = std::clamp(x1, 0.f, width - 1.f);
    y1 = std::clamp(y1, 0.f, height - 1.f);
    x2 = std::clamp(x2, 0.f, width - 1.f);
    y2 = std::clamp(y2, 0.f, height - 1.f);
  }
};

} // namespace spotlight

#endif // FACE_UTILS_HPP
