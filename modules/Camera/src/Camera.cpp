// Copyright 2017 Caoyang Jiang

#include "Jcy/Camera/Camera.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <iostream>
#include <string>
#include <vector>
#include "libuvc/libuvc.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
uvc_context_t* Camera::globalctx_ = nullptr;

uvc_device_t** Camera::devicelist_ = nullptr;

int Camera::gid_ = 0;

bool Camera::InitializeUVCContext()
{
  uvc_error_t ret;

  if (globalctx_ != nullptr)
  {
    std::cout << "[Warn]: uvc context already initialized. " << std::endl;
    return true;
  }

  ret = uvc_init(&globalctx_, nullptr);

  if (ret < 0)
  {
    std::cout << "[ERROR]: Initialize uvc context failed. " << std::endl;
    return false;
  }
  else
  {
    std::cout << "[Message]: Initialize uvc context sucessful." << std::endl;
  }

  return true;
}

void Camera::DestroyUVCContext()
{
  if (globalctx_ != nullptr) uvc_exit(globalctx_);
}

const uvc_context_t* Camera::GetUVCContext()
{
  return globalctx_;
}

bool Camera::CreateDeviceList()
{
  if (devicelist_ == nullptr)
  {
    uvc_error_t ret = uvc_get_device_list(globalctx_, &devicelist_);

    if (ret < 0)
    {
      std::cout << "[ERROR]: Get device list failed. " << std::endl;
      return false;
    }
    else
    {
      std::cout << "[Message]: Get device list  sucessful." << std::endl;
    }
  }

  return true;
}

std::vector<uvc_device_t*> Camera::GetDevicesList()
{
  std::vector<uvc_device_t*> devices;
  uvc_device_t** ptr = devicelist_;

  while (*ptr != nullptr)
  {
    devices.push_back(*ptr);
    ptr++;
  }

  return devices;
}

void Camera::DestroyDeviceList()
{
  uvc_free_device_list(devicelist_, 1);
}

void Camera::StreamCallback(uvc_frame_t* frame, void*)
{
  cv::Mat mjpeg = cv::Mat(
      1, frame->data_bytes, CV_8UC1, reinterpret_cast<uint8_t*>(frame->data));

  cv::Mat img = cv::imdecode(mjpeg, 1);
  if (img.data == nullptr)
  {
    std::cout << "[ERROR]: Parsing mjpeg failed." << std::endl;
  }

  cv::namedWindow("Test", CV_WINDOW_AUTOSIZE);
  cv::imshow("Test", img);
  cv::waitKey(10);

  // uvc_frame_t* bgr;
  // uvc_error_t ret;
  // bgr = uvc_allocate_frame(frame->width * frame->height * 3);

  // if (!bgr)
  // {
  //   printf("unable to allocate bgr frame!");
  //   return;
  // }
  // ret = uvc_any2bgr(frame, bgr);
  // if (ret)
  // {
  //   uvc_perror(ret, "uvc_any2bgr");
  //   uvc_free_frame(bgr);
  //   return;
  // }

  // IplImage* cvImg =
  //     cvCreateImageHeader(cvSize(bgr->width, bgr->height), IPL_DEPTH_8U, 3);
  // cvSetData(cvImg, bgr->data, bgr->width * 3);
  // cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);
  // cvShowImage("Test", cvImg);
  // cvWaitKey(10);
  // cvReleaseImageHeader(&cvImg);
  // uvc_free_frame(bgr);
}

Camera::Camera()
{
}

Camera::~Camera()
{
}

bool Camera::Initialize(uvc_device_t* camdev,
                        int framerate,
                        int width,
                        int height)
{
  uvc_error_t ret;

  if (globalctx_ == nullptr)
  {
    std::cout << "[ERROR]: UVC context not allocated." << std::endl;
    return false;
  }

  camid_ = gid_;
  gid_++;

  ret = uvc_open(camdev, &camdevh_);
  if (ret < 0)
  {
    std::cout << "[ERROR]: Open camera " << camid_ << " failed." << std::endl;
    return false;
  }
  else
  {
    uvc_print_diag(camdevh_, stderr);
  }

  ret = uvc_get_stream_ctrl_format_size(
      camdevh_, &camctrl_, UVC_FRAME_FORMAT_MJPEG, width, height, framerate);

  if (ret < 0)
  {
    std::cout << "[Error]: Get stream control failed. " << std::endl;
    Destroy();
    return -1;
  }
  else
  {
    uvc_set_ae_mode(camdevh_, 1);
    uvc_print_stream_ctrl(&camctrl_, stderr);
  }

  camdev_    = camdev;
  framerate_ = framerate;
  width_     = width;
  height_    = height;

  return true;
}

void Camera::Destroy()
{
  uvc_close(camdevh_);
  uvc_unref_device(camdev_);
}

void Camera::StartCapture()
{
  uvc_error_t ret = uvc_start_streaming(
      camdevh_, &camctrl_, Camera::StreamCallback, nullptr, 0);

  if (ret < 0)
  {
    throw std::runtime_error("Start streaming failed.");
  }
}

void Camera::StopCapture()
{
  uvc_stop_streaming(camdevh_);
}

int Camera::GetWidth() const
{
  return width_;
}

int Camera::GetHeight() const
{
  return height_;
}

int Camera::GetFrameRate() const
{
  return framerate_;
}

const std::vector<uint8_t>& Camera::GetCurrFrame() const
{
  return buffer_;
}

int Camera::GetCameraID() const
{
  return camid_;
}

}  // namespace jcy