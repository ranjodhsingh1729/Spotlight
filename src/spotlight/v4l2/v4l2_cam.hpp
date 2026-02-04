/**
 * @file v4l2_cam.hpp
 * @author Ranjodh Singh
 *
 * @brief V4L2_CAM.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef V4L2_CAM_HPP
#define V4L2_CAM_HPP

#include <memory>

#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <spotlight/v4l2/v4l2.hpp>
#include <spotlight/formats/yuyv.hpp>
#include <spotlight/formats/jpeg.hpp>


namespace spotlight {

class V4L2Camera
{
 public:
  static constexpr v4l2_memory MEM_TYPE = V4L2_MEMORY_MMAP;
  static constexpr v4l2_buf_type BUF_TYPE = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  using Device = V4L2Device<MEM_TYPE, BUF_TYPE>;

  V4L2Camera(
      const std::string& device_path,
      const DeviceConfig& config,
      const int n_buffers = 4
  )
    : dev(device_path, config, n_buffers)
  {
    switch (config.fourcc)
    {
      case (V4L2_PIX_FMT_YUYV):
        converter = std::make_unique<ConverterYUYV>(
          config.width, config.height, 95
        );
        break;
      case (V4L2_PIX_FMT_MJPEG):
        converter = std::make_unique<ConverterJPEG>(
          config.width, config.height, 95
        );
        break;
      default:
        throw_err("Unsupported V4L2_PIX_FMT for: " + device_path);
        break;
    }
  }

  void invoke(void* data)
  {
    v4l2_buffer buffer{};
    buffer.type = BUF_TYPE;
    buffer.memory = MEM_TYPE;

    // TODO: Spec says maybe we can ignore EIO
    if (ioctl(dev.fd, VIDIOC_DQBUF, &buffer) < 0)
      throw std::runtime_error(
        "Failed VIDIOC_DQBUF in v4l2 device" + dev.device_path
      );

    size_t buf_len = buffer.bytesused;
    converter->decode(
      (uint8_t*)dev.buffers[buffer.index].ptr,
      (uint8_t*)data,
      buf_len
    );

    if (ioctl(dev.fd, VIDIOC_QBUF, &buffer) < 0)
      throw std::runtime_error(
        "Failed VIDIOC_QBUF in v4l2 device" + dev.device_path
      );
  }

  Device dev;
  std::unique_ptr<Converter> converter;
};

} // namespace spotlight

#endif // V4L2_CAM_HPP
