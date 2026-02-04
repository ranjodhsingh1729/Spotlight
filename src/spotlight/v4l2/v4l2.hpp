/**
 * @file v4l2.hpp
 * @author Ranjodh Singh
 *
 * @brief V4L2.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef V4L2_HPP
#define V4L2_HPP

#include <vector>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <spotlight/config/config.hpp>
#include <spotlight/utils/error_utils.hpp>


namespace spotlight {

template<v4l2_memory MEM_TYPE, v4l2_buf_type BUF_TYPE>
class V4L2Device
{
 public:
  V4L2Device(
      const std::string& device_path,
      const DeviceConfig& config,
      const int n_buffers = 4
  )
    : fd(-1), device_path(device_path),
      config(config), n_buffers(n_buffers)
  {
    struct stat st;
    if (stat(device_path.c_str(), &st) < 0)
      throw_errno("Failed to identify v4l2 device: " + device_path);

    if (!S_ISCHR(st.st_mode))
    {
      throw_err(device_path + " is not a v4l2 device");
    }

    fd = open(device_path.c_str(), O_RDWR);
    if (fd < 0)
      throw_errno("Failed to open v4l2 device: " + device_path);

    init_device();
    start_io();
  }

  virtual ~V4L2Device()
  {
    stop_io();
    uninit_device();
    if (!(fd < 0))
        close(fd);
  }

  void check_caps()
  {
    struct v4l2_capability cap{};

    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
      throw_errno("Failed VIDIOC_QUERYCAP for: " + device_path);

    // Native (Current Node) vs Emulated (Other Nodes) Capabilities?
    if (!(cap.capabilities & V4L2_CAP_DEVICE_CAPS))
      log_err("V4L2_CAP_DEVICE_CAPS not supported: " + device_path);

    uint32_t caps = (cap.capabilities & V4L2_CAP_DEVICE_CAPS)
                    ? cap.device_caps
                    : cap.capabilities;

    switch (BUF_TYPE)
    {
      case (V4L2_BUF_TYPE_VIDEO_CAPTURE):
        if (!(caps & V4L2_CAP_VIDEO_CAPTURE))
          throw_err("Video Capture not supported by: " + device_path);
        break;
      case (V4L2_BUF_TYPE_VIDEO_OUTPUT):
        if (!(caps & V4L2_CAP_VIDEO_OUTPUT))
          throw_err("Video Output not supported by: " + device_path);
        break;
      default:
        throw_err("Invalid Capability requested for: " + device_path);
    }

    if (!(caps & V4L2_CAP_STREAMING))
      throw_err("Streaming IO not supported by: " + device_path);
  }

  void set_format()
  {
    struct v4l2_format fmt{};
    fmt.type = BUF_TYPE;

    if (ioctl(fd, VIDIOC_G_FMT, &fmt) < 0)
      throw_errno("Failed VIDIOC_G_FMT for: " + device_path);

    fmt.fmt.pix.width = (uint32_t)config.width;
    fmt.fmt.pix.height = (uint32_t)config.height;
    fmt.fmt.pix.pixelformat = (uint32_t)config.fourcc;

    // Progressive video (as opposed to Interlaced)
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0)
      throw_errno("Failed VIDIOC_S_FMT for: " + device_path);

    // Condition for failure
    if (
      fmt.fmt.pix.field != V4L2_FIELD_NONE ||
      fmt.fmt.pix.pixelformat != (unsigned int)config.fourcc
    )
    {
      throw_err("Format rejected by: " + device_path);
    }

    // Not a condition for failiure!
    if (
      fmt.fmt.pix.width != (unsigned int)config.width ||
      fmt.fmt.pix.height != (unsigned int)config.height 
    )
    {
      log_err("Resolution rejected by: " + device_path);
    }
  }

  void set_fps()
  {
    if (BUF_TYPE != V4L2_BUF_TYPE_VIDEO_CAPTURE)
    {
      log_err("set_fps called for non capture: " + device_path);
      return;
    }

    struct v4l2_streamparm param{};
    param.type = BUF_TYPE;

    if (ioctl(fd, VIDIOC_G_PARM, &param) < 0)
    {
      log_errno("Failed VIDIOC_G_PARM for: " + device_path);
      return;
    }

    if (param.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
    {
      param.parm.capture.timeperframe.numerator = 1;
      param.parm.capture.timeperframe.denominator = (uint32_t)config.fps;
    }
    else
    {
      log_err("V4L2_CAP_TIMEPERFRAME not supported by: " + device_path);
    }

    if (ioctl(fd, VIDIOC_S_PARM, &param) < 0)
    {
      log_errno("Failed VIDIOC_S_PARM for: " + device_path);
      return;
    }

    // Not a condition for failiure!
    if (
      param.parm.capture.timeperframe.numerator != (uint32_t)1 || 
      param.parm.capture.timeperframe.denominator != (uint32_t)config.fps
    )
    {
      log_err("FPS rejected by: " + device_path);
    }
  }

  void init_device()
  {
    check_caps();
    set_format();
    if (BUF_TYPE == V4L2_BUF_TYPE_VIDEO_CAPTURE)
      set_fps();

    v4l2_requestbuffers reqbuf{};

    reqbuf.type = BUF_TYPE;
    reqbuf.memory = MEM_TYPE;
    reqbuf.count = n_buffers;

    if (ioctl(fd, VIDIOC_REQBUFS, &reqbuf) < 0)
      throw_errno("Failed VIDIOC_REQBUFS for: " + device_path);

    if (reqbuf.count < 1)
      throw_err("VIDIOC_REQBUFS count less than one for: " + device_path);

    buffers.resize(reqbuf.count, Buffer{nullptr, 0});

    for (size_t i = 0; i < buffers.size(); i++)
    {
      v4l2_buffer buffer{};

      buffer.index = i;
      buffer.type = reqbuf.type;
      buffer.memory = reqbuf.memory;

      if (ioctl(fd, VIDIOC_QUERYBUF, &buffer) < 0)
        throw_errno("Failed VIDIOC_QUERYBUF for: " + device_path);

      void* mapped = mmap(
        nullptr,
        buffer.length,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        buffer.m.offset
      );

      if (mapped == MAP_FAILED)
      {
        uninit_device();
        throw_errno("Failed to mmap buffers for: " + device_path);
      }

      buffers[i].ptr = mapped;  
      buffers[i].length = buffer.length;

      // Should only be used in output mode
      buffers[i].bytesused = buffer.bytesused;
    }
  }

  void uninit_device() noexcept
  {
    for (auto& buffer: buffers)
      if (buffer.ptr != MAP_FAILED && buffer.ptr != nullptr)
        if (munmap(buffer.ptr, buffer.length) < 0)
          log_errno("Failed to munmap buffers for: " + device_path);
  }

  void start_io()
  {
    v4l2_buf_type type = BUF_TYPE;
    for (size_t i = 0; i < buffers.size(); i++)
    {
      struct v4l2_buffer buf{};
      buf.index = i;
      buf.type = BUF_TYPE;
      buf.memory = MEM_TYPE;

      if (BUF_TYPE == V4L2_BUF_TYPE_VIDEO_OUTPUT)
      {
        buf.length = buffers[i].length;
        buf.bytesused = buffers[i].bytesused;
      }

      if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
        throw_errno("Failed VIDIOC_QBUF for: " + device_path);
    }
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0)
      throw_errno("Failed VIDIOC_STREAMON for: " + device_path);
  }

  void stop_io() noexcept
  {
    v4l2_buf_type type = BUF_TYPE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
      log_errno("Failed VIDIOC_STREAMOFF for: " + device_path);
  }


  int fd;
  struct Buffer
  {
    void* ptr;
    size_t length;
    size_t bytesused;
  };
  std::vector<Buffer> buffers;

  const std::string& device_path;
  const DeviceConfig& config;
  const int n_buffers;
};

} // namespace spotlight

#endif // V4L2_HPP
