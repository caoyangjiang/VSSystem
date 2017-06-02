// Copyright 2017 Caoyang Jiang

#include "Jcy/Camera/MultiCamera.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <errno.h>
#include <fcntl.h>
#include <libv4l2.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
// #include "libuvc/libuvc.h"
#include "opencv2/core/core.hpp"
#include "opencv2/core/cuda.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/photo/cuda.hpp"
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
MultiCamera::MultiCamera()
{
}

MultiCamera::~MultiCamera()
{
}

bool MultiCamera::xioctl(int fd, size_t request, void* arg)
{
  int ret;
  do
  {
    ret = v4l2_ioctl(fd, request, arg);
  } while (ret == -1 && ((errno == EINTR) || (errno == EAGAIN)));

  if (ret == -1)
  {
    std::cout << "[ERROR]: xioctl failed with " << errno << std::endl;
    return false;
  }

  return true;
}

void MultiCamera::StreamThread()
{
  int largestfd = *std::max_element(fds_.begin(), fds_.end());
  fd_set fdset;
  struct timeval tv;
  struct v4l2_buffer v4l2buf;

  for (;;)
  {
    int ret = -1;
    do
    {
      tv.tv_sec  = 2;
      tv.tv_usec = 0;

      FD_ZERO(&fdset);
      for (auto fd : fds_)
      {
        FD_SET(fd, &fdset);
      }
      ret = select(largestfd + 1, &fdset, nullptr, nullptr, &tv);
    } while ((ret == -1 && (errno = EINTR)));

    if (ret == -1)
    {
      std::cout << "[ERROR]: select failed. Thread exit." << std::endl;
      return;
    }

    for (auto fd : fds_)
    {
      if (FD_ISSET(fd, &fdset))
      {
        std::memset(&v4l2buf, 0, sizeof(v4l2buf));
        v4l2buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2buf.memory = V4L2_MEMORY_MMAP;
        xioctl(fd, VIDIOC_DQBUF, reinterpret_cast<void*>(&v4l2buf));

        cv::Mat frame;
        cv::Mat cvmat = cv::Mat(
            height_,
            width_,
            CV_8UC3,
            reinterpret_cast<void*>(devicefrmbuf_[fd][v4l2buf.index].addr));

        frame                  = cv::imdecode(cvmat, 1);
        std::string windowname = std::string("Window for device ") + id_[fd];
        cv::namedWindow(windowname, CV_WINDOW_AUTOSIZE);
        cv::imshow(windowname, frame);
        cv::waitKey(10);
        xioctl(fd, VIDIOC_QBUF, &v4l2buf);
      }
    }
  }
}

bool MultiCamera::Initialize(const std::vector<std::string>& devices,
                             int framerate,
                             int width,
                             int height)
{
  int initstatus = 1;

  if (devices.size() == 0)
  {
    std::cout << "[ERROR]: No device specified." << std::endl;
    return false;
  }
  else
  {
    fds_.assign(devices.size(), -1);
  }

  // Open devices
  for (size_t devid = 0; devid < devices.size(); devid++)
  {
    int fd = v4l2_open(devices[devid].c_str(), O_RDWR | O_NONBLOCK, 0);
    if (fd < 0)
    {
      for (size_t c = 0; c < devid; c++)
      {
        v4l2_close(fds_[c]);
      }
      std::cout << "[ERROR]: Cannot open device." << std::endl;
      return false;
    }
    else
    {
      fds_[devid] = fd;
      id_[fd]     = devices[devid];
    }
  }

  // Negotiate format
  struct v4l2_format vformat;
  std::memset(&vformat, 0, sizeof(vformat));
  vformat.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  vformat.fmt.pix.width       = width;
  vformat.fmt.pix.height      = height;
  vformat.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  vformat.fmt.pix.field       = V4L2_FIELD_NONE;
  for (auto fd : fds_)
  {
    xioctl(fd, VIDIOC_S_FMT, &vformat);

    if (vformat.fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG)
    {
      std::cout << "[ERROR]: libv4l2 do not accept V4L2_PIX_FMT_MJPEG."
                << std::endl;
      initstatus = -1;
      break;
    }

    if ((vformat.fmt.pix.width != static_cast<uint32_t>(width)) ||
        (vformat.fmt.pix.height != static_cast<uint32_t>(height)))
    {
      std::cout << "[ERROR]: libv4l2 do not accept width " << width
                << " height " << height << "." << std::endl;
      initstatus = -1;
      break;
    }
  }

  if (initstatus == -1)
  {
    for (auto fd : fds_)
    {
      v4l2_close(fd);
    }
    return false;
  }

  // Request buffer
  struct v4l2_requestbuffers v4l2reqbuf;
  std::memset(&v4l2reqbuf, 0, sizeof(v4l2reqbuf));
  v4l2reqbuf.count  = kFRMBUFPERDEVICE;
  v4l2reqbuf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  v4l2reqbuf.memory = V4L2_MEMORY_MMAP;
  for (auto fd : fds_)
  {
    xioctl(fd, VIDIOC_REQBUFS, &v4l2reqbuf);
  }

  // Initialize buffer
  for (size_t ifd = 0; ifd < fds_.size(); ifd++)
  {
    devicefrmbuf_[fds_[ifd]].resize(kFRMBUFPERDEVICE);

    for (size_t idx = 0; idx < devicefrmbuf_[fds_[ifd]].size(); idx++)
    {
      struct v4l2_buffer v4l2buf;
      std::memset(&v4l2buf, 0, sizeof(v4l2buf));
      v4l2buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      v4l2buf.memory = V4L2_MEMORY_MMAP;
      v4l2buf.index  = idx;
      xioctl(fds_[ifd], VIDIOC_QUERYBUF, &v4l2buf);

      devicefrmbuf_[fds_[ifd]][idx].size = v4l2buf.length;
      devicefrmbuf_[fds_[ifd]][idx].addr = v4l2_mmap(nullptr,
                                                     v4l2buf.length,
                                                     PROT_READ | PROT_WRITE,
                                                     MAP_SHARED,
                                                     fds_[ifd],
                                                     v4l2buf.m.offset);

      std::cout << reinterpret_cast<uint64_t>(
          devicefrmbuf_[fds_[ifd]][idx].addr);

      if (MAP_FAILED == devicefrmbuf_[fds_[ifd]][idx].addr)
      {
        std::cout << "[ERROR]: Memory mapping failed." << std::endl;

        // Clear buffer for ifd
        for (size_t ridx = 0; ridx < idx; ridx++)
        {
          v4l2_munmap(devicefrmbuf_[fds_[ifd]][idx].addr,
                      devicefrmbuf_[fds_[ifd]][idx].size);
        }
        // Clear buffers for devices before ifd
        for (size_t rfd = 0; rfd < ifd; rfd++)
        {
          for (size_t ridx = 0; ridx < devicefrmbuf_[rfd].size(); ridx++)
          {
            v4l2_munmap(devicefrmbuf_[fds_[rfd]][ridx].addr,
                        devicefrmbuf_[fds_[rfd]][ridx].size);
          }
        }

        initstatus = -1;
        goto Done1;
      }
    }
  }

Done1:
  if (initstatus == -1)
  {
    for (auto fd : fds_)
    {
      v4l2_close(fd);
    }
    return false;
  }

  // Queue buffer
  for (auto fd : fds_)
  {
    for (size_t idx = 0; idx < devicefrmbuf_[fd].size(); idx++)
    {
      struct v4l2_buffer v4l2buf;
      v4l2buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      v4l2buf.memory = V4L2_MEMORY_MMAP;
      v4l2buf.index  = idx;
      xioctl(fd, VIDIOC_QBUF, &v4l2buf);
    }
  }

  framerate_ = framerate;
  width_     = width;
  height_    = height;

  for (auto fd : fds_)
  {
    internalbuffer_[fd].resize(width * height * 3 / 2);  // yuv420  buffer
    externalbuffer_[fd].resize(width * height * 3 / 2);  // yuv420  buffer
  }

  // Start streaming thread
  streamthread_ = std::thread(&MultiCamera::StreamThread, this);
  return true;
}

void MultiCamera::Destroy()
{
  for (auto fd : fds_)
  {
    for (size_t ridx = 0; ridx < devicefrmbuf_[fd].size(); ridx++)
    {
      v4l2_munmap(devicefrmbuf_[fd][ridx].addr, devicefrmbuf_[fd][ridx].size);
    }
  }

  for (auto fd : fds_)
  {
    v4l2_close(fd);
  }
}

void MultiCamera::StartCapture()
{
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  for (auto fd : fds_)
  {
    xioctl(fd, VIDIOC_STREAMON, &type);
  }
}

void MultiCamera::StopCapture()
{
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  for (auto fd : fds_)
  {
    xioctl(fd, VIDIOC_STREAMON, &type);
  }
}

int MultiCamera::GetWidth() const
{
  return width_;
}

int MultiCamera::GetHeight() const
{
  return height_;
}

int MultiCamera::GetFrameRate() const
{
  return framerate_;
}

const std::map<int, std::vector<uint8_t>>& MultiCamera::GetCurrFrame()
{
  std::unique_lock<std::mutex> qlock(bufferaccess_);
  for (auto fd : fds_)
  {
    std::memcpy(externalbuffer_[fd].data(),
                internalbuffer_[fd].data(),
                internalbuffer_[fd].size());
  }
  qlock.unlock();
  return externalbuffer_;
}

int MultiCamera::GetDeviceIDByName(const std::string& dev) const
{
  for (std::map<int, std::string>::const_iterator it = id_.begin();
       it != id_.end();
       it++)
  {
    if (it->second == dev)
    {
      return it->first;
    }
  }

  return -1;
}

}  // namespace jcy
