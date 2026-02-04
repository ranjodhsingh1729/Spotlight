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


namespace spotlight {

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
  int num_threads;
  int inp_width;
  int inp_height;
  int out_width;
  int out_height;

  uint32_t inp_fourcc;
  uint32_t out_fourcc;
  
  double inp_fps;
  double out_fps;

  std::string inp_path;
  std::string out_path;
  std::string bg_img_path;


  int InpPixels() const { return inp_width * inp_height; }
  int OutPixels() const { return out_width * out_height; }

  DeviceConfig InpConfig() const
  {
    return {
      inp_fourcc,
      inp_width,
      inp_height,
      inp_fps
    };
  }

  DeviceConfig OutConfig() const
  {
    return {
      out_fourcc,
      out_width,
      out_height,
      out_fps
    };
  }
};

} // namespace spotlight

#endif // CONFIG_HPP
