/**
 * @file v4l2_vcam.hpp
 * @author Ranjodh Singh
 *
 * @brief V4L2_VCAM.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef V4L2_VCAM_HPP
#define V4L2_VCAM_HPP

#include <chrono>
#include <thread>

#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <spotlight/v4l2/v4l2.hpp>
#include <spotlight/formats/yuyv.hpp>
#include <spotlight/formats/jpeg.hpp>


namespace spotlight {

class V4L2VirtualCamera
{
 public:
  using clock = std::chrono::steady_clock;
  using duration = std::chrono::steady_clock::duration;

  static constexpr v4l2_memory MEM_TYPE = V4L2_MEMORY_MMAP;
  static constexpr v4l2_buf_type BUF_TYPE = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  using Device = V4L2Device<MEM_TYPE, BUF_TYPE>;

  V4L2VirtualCamera(
      const std::string& device_path,
      const DeviceConfig& config,
      const int n_buffers = 4
  )
    : dev(device_path, config, n_buffers)
  {
    ts = clock::now();
    spf = std::chrono::duration_cast<duration>(
      std::chrono::duration<double>(1.0 / config.fps)
    );

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

    // TODO: Spec says we can ignore EIO
    if (ioctl(dev.fd, VIDIOC_DQBUF, &buffer) < 0)
      throw std::runtime_error(
        "Failed VIDIOC_DQBUF in v4l2 device" + dev.device_path
      );

    ts += spf;
    std::this_thread::sleep_until(ts);

    // Note: This is optional for v4l2loopback
    // if (set_timestamp) {
		// 	struct timespec curTime;
		// 	clock_gettime(CLOCK_MONOTONIC, &curTime);
		// 	buffer.timestamp.tv_sec = curTime.tv_sec;
		// 	buffer.timestamp.tv_usec = curTime.tv_nsec / 1000ULL;
		// } else {
		// 	buffer.timestamp.tv_sec = 0;
		// 	buffer.timestamp.tv_usec = 0;
		// }

    size_t buf_len = dev.buffers[buffer.index].length;
    converter->encode(
      (uint8_t*)data,
      (uint8_t*)dev.buffers[buffer.index].ptr,
      &buf_len
    );
    buffer.bytesused = buf_len;

    if (ioctl(dev.fd, VIDIOC_QBUF, &buffer) < 0)
      throw std::runtime_error(
        "Failed VIDIOC_QBUF in v4l2 device" + dev.device_path
      );
  }

  duration spf;
  clock::time_point ts;

  Device dev;
  std::unique_ptr<Converter> converter;
};

} // namespace spotlight

#endif // V4L2_VCAM_HPP
