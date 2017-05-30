// Copyright 2015 Caoyang Jiang

#include "Jcy/GpuEncoder/GpuVideoEncoder.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <cstring>
#include <iostream>
#include <string>
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
GpuVideoEncoder::GpuVideoEncoder()
{
}

GpuVideoEncoder::~GpuVideoEncoder()
{
}

NVENCSTATUS GpuVideoEncoder::InitCuda(uint32_t deviceID)
{
  CUresult curesult;
  CUdevice device;

  int deviceCount = 0;
  int SMminor = 0, SMmajor = 0;

  curesult = cuInit(0);
  if (curesult != CUDA_SUCCESS)
  {
    std::cout << "cuInit error: " << curesult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  curesult = cuDeviceGetCount(&deviceCount);
  if (curesult != CUDA_SUCCESS)
  {
    std::cout << "cuDeviceGetCount error: " << curesult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  // If dev is negative value, we clamp to 0
  if (static_cast<int>(deviceID) < 0) deviceID = 0;

  if (deviceID > (unsigned int)deviceCount - 1)
  {
    std::cout << "Invalid Device Id = " << deviceID << std::endl;
    return NV_ENC_ERR_INVALID_ENCODERDEVICE;
  }

  curesult = cuDeviceGet(&device, deviceID);
  if (curesult != CUDA_SUCCESS)
  {
    std::cout << "cuDeviceGet error: " << curesult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  curesult = cuDeviceComputeCapability(&SMmajor, &SMminor, deviceID);
  if (curesult != CUDA_SUCCESS)
  {
    std::cout << "cuDeviceComputeCapability error: " << curesult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  if (((SMmajor << 4) + SMminor) < 0x30)
  {
    std::cout << "GPU " << deviceID
              << " does not have NVENC capabilities exiting\n"
              << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  curesult = cuCtxCreate(reinterpret_cast<CUcontext*>(&device_), 0, device);
  if (curesult != CUDA_SUCCESS)
  {
    std::cout << "cuCtxCreate error: " << curesult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }

  curesult = cuCtxPopCurrent(&cudactx_);
  if (curesult != CUDA_SUCCESS)
  {
    std::cout << "cuCtxPopCurrent error: " << curesult << std::endl;
    return NV_ENC_ERR_NO_ENCODE_DEVICE;
  }
  return NV_ENC_SUCCESS;
}

bool GpuVideoEncoder::Initialize(int width,
                                 int height,
                                 int gopsize,
                                 int framerate)

{
  NVENCSTATUS nvstatus = NV_ENC_SUCCESS;

  width_               = width;
  height_              = height;
  framerate_           = framerate;
  gopsize_             = gopsize;
  bitstreambuffersize_ = 2 * 1024 * 1024;
  bitstreambuffer_ =
      std::unique_ptr<uint8_t[]>(new uint8_t[bitstreambuffersize_]);

  if (InitCuda(0) != NV_ENC_SUCCESS) return false;

// Create encoder API function list
#if defined(NV_WINDOWS)  // Windows
  MYPROC NvEncodeAPICreateInstance;

#if defined(_WIN64)  // 64-bit
  if ((hinstlib_ = LoadLibrary(TEXT("nvEncodeAPI64.dll"))) == NULL)
  {
    std::cout << "Load Libary nvEncodeAPI64.dll failed" << std::endl;
    return false;
  }
#else  // 32-bit
  if ((hinstlib_ = LoadLibrary(TEXT("nvEncodeAPI.dll"))) == NULL)
  {
    std::cout << "Load Libary nvEncodeAPI.dll failed" << std::endl;
    return false;
  }
#endif

  NvEncodeAPICreateInstance = static_cast<MYPROC>(
      GetProcAddress(hinstLib, "NvEncodeAPICreateInstance"));
  if (NvEncodeAPICreateInstance == NULL)
  {
    std::cout << "GetProcAddress failed" << std::endl;
    return false;
  }
#endif

  std::memset(&apifunclist_, 0, sizeof(NV_ENCODE_API_FUNCTION_LIST));
  apifunclist_.version = NV_ENCODE_API_FUNCTION_LIST_VER;
  if ((nvstatus = NvEncodeAPICreateInstance(&apifunclist_)) != NV_ENC_SUCCESS)
  {
    std::cout << "[ERROR]: NvEncodeAPICreateInstance failed" << std::endl;
    return false;
  }

  // Create encoder encoding sesssion
  std::memset(&sessionparam_, 0, sizeof(NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS));
  sessionparam_.version    = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;
  sessionparam_.deviceType = NV_ENC_DEVICE_TYPE_CUDA;  // Only option for Linux.
  sessionparam_.device     = device_;
  sessionparam_.apiVersion = NVENCAPI_VERSION;
  if ((nvstatus = apifunclist_.nvEncOpenEncodeSessionEx(
           &sessionparam_, &encoder_)) != NV_ENC_SUCCESS)
  {
    std::cout << "[ERROR]: nvEncOpenEncodeSessionEx failed" << std::endl;
    return false;
  }

  // Create encoder initialization parameters
  encinitparam_.version           = NV_ENC_INITIALIZE_PARAMS_VER;
  encinitparam_.encodeGUID        = NV_ENC_CODEC_H264_GUID;
  encinitparam_.presetGUID        = NV_ENC_PRESET_LOW_LATENCY_HP_GUID;
  encinitparam_.encodeWidth       = width_;
  encinitparam_.encodeHeight      = height_;
  encinitparam_.darWidth          = width_;
  encinitparam_.darHeight         = height_;
  encinitparam_.maxEncodeWidth    = 0;
  encinitparam_.maxEncodeHeight   = 0;
  encinitparam_.frameRateNum      = framerate_;
  encinitparam_.enableEncodeAsync = 0;  // Only option for Linux
  encinitparam_.encodeConfig =
      &encodecfg_;              // This sets encoder configurations.
  encinitparam_.enablePTD = 1;  // Let encoder determine picture type.

  // Substep #1: Get preset parameter using the preset configuration GUID. This
  // step doesn't need modified. To change preset, set the GUID in encinitparam_
  std::memset(&presetcfg_, 0, sizeof(NV_ENC_PRESET_CONFIG));
  presetcfg_.version           = NV_ENC_PRESET_CONFIG_VER;
  presetcfg_.presetCfg.version = NV_ENC_CONFIG_VER;
  if ((nvstatus = apifunclist_.nvEncGetEncodePresetConfig(
           encoder_,
           encinitparam_.encodeGUID,
           encinitparam_.presetGUID,
           &presetcfg_)) != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncGetEncodePresetConfig failed" << std::endl;
    return false;
  }

  // Substep #2: Copy preset configuration to encoder configuration and edit it
  // further. Fine tuning encoder can be done here.
  std::memset(&encodecfg_, 0, sizeof(NV_ENC_CONFIG));
  std::memcpy(&encodecfg_, &presetcfg_, sizeof(NV_ENC_CONFIG));
  encodecfg_.gopLength                 = gopsize_;  // NVENC_INFINITE_GOPLENGTH;
  encodecfg_.frameIntervalP            = 0;         // IPP structure
  encodecfg_.frameFieldMode            = NV_ENC_PARAMS_FRAME_FIELD_MODE_FRAME;
  encodecfg_.mvPrecision               = NV_ENC_MV_PRECISION_HALF_PEL;
  encodecfg_.rcParams.rateControlMode  = NV_ENC_PARAMS_RC_CONSTQP;
  encodecfg_.rcParams.constQP.qpInterP = 28;
  encodecfg_.rcParams.constQP.qpIntra  = 28;
  encodecfg_.rcParams.initialRCQP.qpIntra                 = 24;
  encodecfg_.encodeCodecConfig.h264Config.maxNumRefFrames = 16;
  encodecfg_.encodeCodecConfig.h264Config.chromaFormatIDC = 1;  // YUV420
  encodecfg_.encodeCodecConfig.h264Config.disableDeblockingFilterIDC = 0;
  // encodecfg_.encodeCodecConfig.h264Config.sliceMode                  = 3;
  // encodecfg_.encodeCodecConfig.h264Config.sliceModeData              = 1;
  encodecfg_.encodeCodecConfig.h264Config.fmoMode = NV_ENC_H264_FMO_DISABLE;
  encodecfg_.encodeCodecConfig.h264Config.adaptiveTransformMode =
      NV_ENC_H264_ADAPTIVE_TRANSFORM_DISABLE;
  // Initialize encoder with initialization parameters
  if ((nvstatus = apifunclist_.nvEncInitializeEncoder(
           encoder_, &encinitparam_)) != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncInitializeEncoder failed" << std::endl;
    return false;
  }

  // Create input buffer
  std::memset(&inputbufferparam_, 0, sizeof(NV_ENC_CREATE_INPUT_BUFFER));
  inputbufferparam_.version    = NV_ENC_CREATE_INPUT_BUFFER_VER;
  inputbufferparam_.width      = width_;
  inputbufferparam_.height     = height_;
  inputbufferparam_.memoryHeap = NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;
  inputbufferparam_.bufferFmt  = NV_ENC_BUFFER_FORMAT_IYUV;
  if ((nvstatus = apifunclist_.nvEncCreateInputBuffer(
           encoder_, &inputbufferparam_)) != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncCreateInputBuffer failed" << std::endl;
    return false;
  }
  inputbuffer_ = inputbufferparam_.inputBuffer;

  // Create output buffer
  std::memset(&outputbufferparam_, 0, sizeof(NV_ENC_CREATE_BITSTREAM_BUFFER));
  outputbufferparam_.version    = NV_ENC_CREATE_BITSTREAM_BUFFER_VER;
  outputbufferparam_.size       = 2 * 1024 * 1024;  // No idea why
  outputbufferparam_.memoryHeap = NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;
  if ((nvstatus = apifunclist_.nvEncCreateBitstreamBuffer(
           encoder_, &outputbufferparam_)) != NV_ENC_SUCCESS)
  {
    std::cout << "nvEncCreateBitstreamBuffer failed" << std::endl;
    return 1;
  }
  outputbuffer_ = outputbufferparam_.bitstreamBuffer;

  return true;
}

bool GpuVideoEncoder::Destroy()
{
  NVENCSTATUS nvstatus = NV_ENC_SUCCESS;

  // Destroy input buffer
  if (inputbuffer_)
  {
    if ((nvstatus = apifunclist_.nvEncDestroyInputBuffer(
             encoder_, inputbuffer_)) != NV_ENC_SUCCESS)
    {
      std::cout << "nvEncDestroyInputBuffer failed" << std::endl;
      return 1;
    }
  }

  // Destroy output buffer
  if (outputbuffer_)
  {
    if ((nvstatus = apifunclist_.nvEncDestroyBitstreamBuffer(
             encoder_, outputbuffer_)) != NV_ENC_SUCCESS)
    {
      std::cout << "[ERROR]: nvEncDestroyBitstreamBuffer failed" << std::endl;
      return 1;
    }
  }

  // Terminate Encoding Session
  if ((nvstatus = apifunclist_.nvEncDestroyEncoder(encoder_)) != NV_ENC_SUCCESS)
  {
    std::cout << "[ERROR]: nvEncDestroyEncoder failed" << std::endl;
    return false;
  }

  return true;
}

bool GpuVideoEncoder::EncodeAFrame(const uint8_t* buffer)
{
  NVENCSTATUS nvstatus = NV_ENC_SUCCESS;
  NV_ENC_LOCK_INPUT_BUFFER inputbufferlocker;

  // Lock input buffer for loading input frame
  std::memset(&inputbufferlocker, 0, sizeof(NV_ENC_LOCK_INPUT_BUFFER));
  inputbufferlocker.version     = NV_ENC_LOCK_INPUT_BUFFER_VER;
  inputbufferlocker.inputBuffer = inputbuffer_;
  if ((nvstatus = apifunclist_.nvEncLockInputBuffer(
           encoder_, &inputbufferlocker)) != NV_ENC_SUCCESS)
  {
    std::cout << "[ERROR]: nvEncLockInputBuffer failed" << std::endl;
    return 1;
  }

  // Loading a frame into input buffer
  std::memcpy(
      inputbufferlocker.bufferDataPtr, buffer, width_ * height_ * 3 / 2);

  // Unlock for encoding
  if ((nvstatus = apifunclist_.nvEncUnlockInputBuffer(
           encoder_, inputbuffer_)) != NV_ENC_SUCCESS)
  {
    std::cout << "[ERROR]: nvEncUnlockInputBuffer failed" << std::endl;
    return 1;
  }

  // Prepare picture for encoding
  std::memset(&encodepicparam_, 0, sizeof(NV_ENC_PIC_PARAMS));
  encodepicparam_.version         = NV_ENC_PIC_PARAMS_VER;
  encodepicparam_.inputWidth      = width_;
  encodepicparam_.inputHeight     = height_;
  encodepicparam_.inputPitch      = width_;
  encodepicparam_.inputBuffer     = inputbuffer_;
  encodepicparam_.outputBitstream = outputbuffer_;
  encodepicparam_.bufferFmt       = NV_ENC_BUFFER_FORMAT_IYUV;
  encodepicparam_.pictureStruct   = NV_ENC_PIC_STRUCT_FRAME;
  encodepicparam_.inputTimeStamp  = 0;

  // Encode a frame
  if ((nvstatus = apifunclist_.nvEncEncodePicture(
           encoder_, &encodepicparam_)) != NV_ENC_SUCCESS)
  {
    std::cout << "[ERROR]: nvEncEncodePicture failed" << std::endl;
    return 1;
  }

  return true;
}

bool GpuVideoEncoder::GetBitStream(const uint8_t*& bitstream, size_t& bssize)
{
  NVENCSTATUS nvstatus = NV_ENC_SUCCESS;
  NV_ENC_LOCK_BITSTREAM outputbufferlocker;

  // Lock output buffer for reading coded bitstream
  std::memset(&outputbufferlocker, 0, sizeof(NV_ENC_LOCK_BITSTREAM));
  outputbufferlocker.version         = NV_ENC_LOCK_BITSTREAM_VER;
  outputbufferlocker.outputBitstream = outputbuffer_;
  if ((nvstatus = apifunclist_.nvEncLockBitstream(
           encoder_, &outputbufferlocker)) != NV_ENC_SUCCESS)
  {
    std::cout << "[ERROR]: nvEncLockBitstream failed " << nvstatus << std::endl;
    return 1;
  }

  // Load coded bitstream
  if (outputbufferlocker.bitstreamSizeInBytes >
      static_cast<uint32_t>(bitstreambuffersize_))
  {
    std::cout << "[ERROR]: Internal bitstream buffer is too small" << std::endl;
    return false;
  }

  std::memcpy(bitstreambuffer_.get(),
              outputbufferlocker.bitstreamBufferPtr,
              outputbufferlocker.bitstreamSizeInBytes);

  // Unlock output buffer for future use
  if ((nvstatus = apifunclist_.nvEncUnlockBitstream(encoder_, outputbuffer_)) !=
      NV_ENC_SUCCESS)
  {
    std::cout << "[ERROR]: nvEncUnlockBitstream failed" << std::endl;
    return 1;
  }

  bitstream = reinterpret_cast<uint8_t*>(bitstreambuffer_.get());
  bssize    = static_cast<size_t>(outputbufferlocker.bitstreamSizeInBytes);
  return true;
}
}  // namespace jcy
