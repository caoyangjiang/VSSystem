// Copyright 2017 Caoyang Jiang

#ifndef MODULES_CAMERA_INCLUDE_JCY_CAMERA_MULTICAMERA_H_
#define MODULES_CAMERA_INCLUDE_JCY_CAMERA_MULTICAMERA_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <libv4l2.h>
#include <linux/videodev2.h>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "libuvc/libuvc.h"
// #include "opencv2/opencv.hpp"
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
class MultiCamera
{
 public:
  MultiCamera();
  ~MultiCamera();

  bool Initialize(const std::vector<std::string>& devices,
                  int framerate,
                  int width,
                  int height);

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
   * @brief      Gets the device id by name.
   *
   * @param[in]  dev   The development
   *
   * @return     The device id by name.
   */
  int GetDeviceIDByName(const std::string& dev) const;

  /**
   * @brief      Gets the curr frame.
   *
   * @return     The curr frame.
   */
  const std::vector<uint8_t>& GetCurrFrame(int deviceid);

  /**
   * @brief      Gets the device IDs.
   *
   * @return     The device IDs.
   */
  inline const std::vector<int> GetDeviceIDs() const
  {
    return fds_;
  }

 private:
  bool xioctl(int fd, size_t request, void* arg);
  void StreamThread();

 public:
  static const int kMAXDEVICECNT    = 4;
  static const int kFRMBUFPERDEVICE = 2;

 private:
  std::thread streamthread_;
  std::mutex bufferaccess_;
  std::map<int, std::vector<uint8_t>> internalbuffer_;
  std::map<int, std::vector<uint8_t>> externalbuffer_;

  typedef struct FrameBuffer_t
  {
    void* addr;
    size_t size;
  } FrameBuffer_t;

  std::vector<int> fds_;
  std::map<int, std::string> id_;
  std::map<int, std::vector<FrameBuffer_t>> devicefrmbuf_;

  int width_     = -1;
  int height_    = -1;
  int framerate_ = -1;
  int camid_     = -1;
};

}  // namespace jcy

#endif  // MODULES_CAMERA_INCLUDE_JCY_CAMERA_MULTICAMERA_H_
