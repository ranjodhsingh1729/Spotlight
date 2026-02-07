/**
 * @file config.hpp
 * @author Ranjodh Singh
 *
 * @brief CONFIG.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <cstdint>
#include <spotlight/config/defaults.hpp>
#include <spotlight/utils/error_utils.hpp>

namespace spotlight {

inline uint32_t get_fourcc(const std::string_view s)
{
  if (s.size() != 4)
    throw_err("FOURCC must be exactly 4 characters");
  return v4l2_fourcc(s[0], s[1], s[2], s[3]);
}

enum class PipelineMode {
  BLUR,
  IMAGE,
  VIDEO, // TODO: SUPPORT THIS!
};

struct DeviceConfig
{ 
  uint32_t fourcc;
  int width;
  int height;
  double fps;

  bool operator==(const DeviceConfig& other) const
  {
    return (
      fourcc == other.fourcc &&
      width == other.width &&
      height == other.height &&
      fps == other.fps
    );
  }

  bool operator!=(const DeviceConfig& other) const
  {
    return !(*this == other);
  }
};


struct PipelineConfig
{
  PipelineMode mode = MODE;
  int n_threads = N_THREADS;

  int in_w = IN_W;
  int in_h = IN_H;
  int out_w = OUT_W;
  int out_h = OUT_H;

  uint32_t in_fmt = IN_FMT;
  uint32_t out_fmt = OUT_FMT;
  
  double in_fps = IN_FPS;
  double out_fps = OUT_FPS;

  std::string in_dev = IN_DEV;
  std::string out_dev = OUT_DEV;
  std::string bg_img = BG_IMG;


  int InpPixels() const { return in_w * in_h; }
  int OutPixels() const { return out_w * out_h; }

  DeviceConfig InpConfig() const
  {
    return {
      in_fmt,
      in_w,
      in_h,
      in_fps
    };
  }

  DeviceConfig OutConfig() const
  {
    return {
      out_fmt,
      out_w,
      out_h,
      out_fps
    };
  }

  void set(const std::string& key, const std::string& value)
  {
    if (key == "mode")
    {
      if (value == "blur")
        mode = PipelineMode::BLUR;
      else if (value == "image")
        mode = PipelineMode::IMAGE;
      else if (value == "video")
        mode = PipelineMode::VIDEO;
      else
        throw_err("Invalid PipelineMode: " + value);
    }
    else if (key == "n-threads")
    {
      n_threads = std::stoi(value);
    }
    else if (key == "in-w")
    {
      in_w = std::stoi(value);
    }
    else if (key == "in-h")
    {
      in_h = std::stoi(value);
    }
    else if (key == "out-w")
    {
      out_w = std::stoi(value);
    }
    else if (key == "out-h")
    {
      out_h = std::stoi(value);
    }
    else if (key == "in-fmt")
    {
      in_fmt = get_fourcc(value);
    }
    else if (key == "out-fmt")
    {
      out_fmt = get_fourcc(value);
    }
    else if (key == "in-fps")
    {
      in_fps = std::stod(value);
    }
    else if (key == "out-fps")
    {
      out_fps = std::stod(value);
    }
    else if (key == "in-dev")
    {
      in_dev = value;
    }
    else if (key == "out-dev")
    {
      out_dev = value;
    }
    else if (key == "bg-img")
    {
      bg_img = value;
    }
    else
      throw_err("Invalid Option: " + key);
  }
};

} // namespace spotlight

#endif // CONFIG_HPP
