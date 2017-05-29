// Copyright 2017 Caoyang Jiang

#ifndef MODULES_CAMERA_INCLUDE_JCY_CAMERA_CAMERA_H_
#define MODULES_CAMERA_INCLUDE_JCY_CAMERA_CAMERA_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <string>
#include <vector>
#include "libuvc/libuvc.h"
// #include "opencv2/opencv.hpp"
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
class Camera
{
 public:
  static void StreamCallback(uvc_frame_t* frame, void* ptr);

  /**
   * @brief      Initialize global UVC context.
   *a
   * @return     True if successful, false otherwise.
   */
  static bool InitializeUVCContext();

  /**
   * @brief      Destroy global UVC context.
   */
  static void DestroyUVCContext();

  /**
   * @brief      Gets the uvc context.
   *
   * @return     The uvc context.
   */
  static const uvc_context_t* GetUVCContext();

  /**
   * @brief      Gets the device list. Must call after initialize context.
   *
   * @return     The device list.
   */
  static bool CreateDeviceList();

  /**
   * @brief      Gets the devices list.
   *
   * @return     The devices list.
   */
  static std::vector<uvc_device_t*> GetDevicesList();

  /**
   * @brief      free device list
   */
  static void DestroyDeviceList();

 public:
  Camera();
  ~Camera();

  bool Initialize(uvc_device_t* camdev, int framerate, int width, int height);

  /**
   * @brief    Destroy Camera and free memory.
   */
  void Destroy();

  /**
   * @brief      Starts a capture.
   */
  void StartCapture();

  /**
   * @brief      Stops a capture.
   */
  void StopCapture();

  /**
   * @brief      Gets the width.
   *
   * @return     The width.
   */
  int GetWidth() const;

  /**
   * @brief      Gets the height.
   *
   * @return     The height.
   */
  int GetHeight() const;

  /**
   * @brief      Gets the frame rate.
   *
   * @return     The frame rate.
   */
  int GetFrameRate() const;

  /**
   * @brief      Gets the curr frame.
   *
   * @return     The curr frame.
   */
  const std::vector<uint8_t>& GetCurrFrame() const;

  /**
   * @brief      Gets the camera id.
   *
   * @return     The camera id.
   */
  int GetCameraID() const;

 private:
  int width_     = -1;
  int height_    = -1;
  int framerate_ = -1;
  int camid_     = -1;
  std::vector<uint8_t> buffer_;

  static uvc_context_t* globalctx_;
  static uvc_device_t** devicelist_;
  static int gid_;

  uvc_device_t* camdev_;
  uvc_device_handle_t* camdevh_;
  uvc_stream_ctrl_t camctrl_;
};

}  // namespace jcy

#endif  // MODULES_CAMERA_INCLUDE_JCY_CAMERA_CAMERA_H_
