// Copyright 2017 Caoyang Jiang

#ifndef MODULES_CPUENCODER_INCLUDE_JCY_CPUENCODER_VIDEOENCODER_H_
#define MODULES_CPUENCODER_INCLUDE_JCY_CPUENCODER_VIDEOENCODER_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <stdint.h>
#include <x264.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
JCY_WINDOWS_DISABLE_ALL_WARNING

namespace jcy
{
/**
 * @brief      Class for encoding video.
 * @ingroup    CodecUtility
 */
class VideoEncoder
{
 public:
  /**
   * @brief      Initialize video encoder.
   *
   * @param[in]  width      The width
   * @param[in]  height     The height
   * @param[in]  gopsize    The gop size
   * @param[in]  framerate  The framerate
   *
   * @return     True if initialization successful, false otherwise.
   */
  JCY_CPUENCODER_DLL bool Initialize(int width,
                                     int height,
                                     int gopsize,
                                     int framerate);

  /**
   * @brief      Destroy video encoder.
   */
  JCY_CPUENCODER_DLL void Destroy();

  /**
   * @brief      Encode a frame
   *
   * @param[in]  buffer  The frame buffer
   *
   * @return     True if encoding successful, false otherwise.
   */
  JCY_CPUENCODER_DLL bool EncodeAFrame(const uint8_t* buffer);

  /**
   * @brief      Encode a frame
   *
   * @param[in]  ybuffer  The ybuffer
   * @param[in]  ubuffer  The ubuffer
   * @param[in]  vbuffer  The vbuffer
   *
   * @return     True if encoding successful, false otherwise.
   */
  JCY_CPUENCODER_DLL bool EncodeAFrame(const uint8_t* ybuffer,
                                       const uint8_t* ubuffer,
                                       const uint8_t* vbuffer);
  /**
   * @brief      Gets the coded bit stream for the input frame.
   *
   * @param[in]  bitstream  The bitstream
   * @param[in]  bssize     The bssize
   */
  JCY_CPUENCODER_DLL void GetBitStream(const uint8_t*& bitstream,
                                       size_t& bssize);

 private:
  int width_      = 0;
  int height_     = 0;
  int framerate_  = 0;
  int gopsize_    = 0;
  int framecnt_   = 0;
  int lplanesize_ = 0;
  int cplanesize_ = 0;
  int uplaneos_   = 0;
  int vplaneos_   = 0;

  x264_param_t param_;
  x264_t* encoder_;
  x264_picture_t picin_;
  x264_picture_t picout_;
  x264_picture_t picout;
  x264_nal_t* nal_;
  std::vector<uint8_t> bs_;
};

}  // namespace jcy

#endif  // MODULES_CPUENCODER_INCLUDE_JCY_CPUENCODER_VIDEOENCODER_H_
