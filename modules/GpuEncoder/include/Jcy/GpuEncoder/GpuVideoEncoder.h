// Copyright 2017 Caoyang Jiang

#ifndef MODULES_GPUENCODER_INCLUDE_JCY_GPUENCODER_GPUVIDEOENCODER_H_
#define MODULES_GPUENCODER_INCLUDE_JCY_GPUENCODER_GPUVIDEOENCODER_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <cuda.h>
#include <memory>
#include <string>
#include "NVCodec/nvEncodeAPI.h"
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
/**
 * @brief      Class for encoding rendered image using NVIDIA hardware encoder.
 *             Input frame for encoding is allocated internally.
 * @ingroup    RenderServer.
 */
class GpuVideoEncoder
{
 public:
  GpuVideoEncoder();
  ~GpuVideoEncoder();

  GpuVideoEncoder& operator=(const GpuVideoEncoder&) = delete;
  GpuVideoEncoder(const GpuVideoEncoder&)            = delete;

  /**
   * @brief      Initialize stream encoder with configuration file.
   *
   * @param[in]  width      The width
   * @param[in]  height     The heighta
   * @param[in]  gopsize    The gopsize
   * @param[in]  framerate  The framerate
   *
   * @return     True if initialization successful, false otherwise.
   */
  bool Initialize(int width, int height, int gopsize, int framerate);

  /**
   * @brief      Destroy this stream encoder.
   *
   * @return     True if destroy successful, false otherwise.
   */
  bool Destroy();

  /**
   * @brief      Feed encoder with one raw frame data.
   *
   * @param[in]  buffer      The frame buffer.
    *
   * @return     True if encoding successful, false otherwise.
   */
  bool EncodeAFrame(const uint8_t* buffer);

  /**
   * @brief      Retreive bitstream for recently encoded frame.
   *
   * @param[out] bitstream  The bitstream.
   * @param[out] bssize     The bitstream size.
   *
   * @return     True if get bitstream successful, false otherwise.
   */
  bool GetBitStream(const uint8_t*& bitstream, size_t& bssize);

 private:
  NVENCSTATUS InitCuda(uint32_t deviceID);

 private:
  int width_     = 0;
  int height_    = 0;
  int framerate_ = 0;
  int gopsize_   = 0;

  CUcontext cudactx_;
  void* device_       = nullptr;
  void* encoder_      = nullptr;  // Similar to FFmpeg AVCodecContext
  void* inputbuffer_  = nullptr;
  void* outputbuffer_ = nullptr;

  NV_ENCODE_API_FUNCTION_LIST apifunclist_;
  NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS sessionparam_;
  NV_ENC_INITIALIZE_PARAMS encinitparam_;  // Similar to FFmpeg AVCodec
  NV_ENC_PRESET_CONFIG presetcfg_;         // Subparameter for encinitparam_
  NV_ENC_CONFIG encodecfg_;                // Subparameter for encinitparam_
  NV_ENC_PIC_PARAMS encodepicparam_;

  // Buffers
  NV_ENC_CREATE_INPUT_BUFFER inputbufferparam_;
  NV_ENC_CREATE_BITSTREAM_BUFFER outputbufferparam_;
  std::unique_ptr<uint8_t[]> bitstreambuffer_;
  int bitstreambuffersize_ = 0;

#if defined(NV_WINDOWS)
  void* hinstlib_;
#endif
};
}  // namespace jcy

#endif  // MODULES_GPUENCODER_INCLUDE_JCY_GPUENCODER_GPUVIDEOENCODER_H_
