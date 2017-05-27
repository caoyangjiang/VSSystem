// Copyright 2017 Caoyang Jiang

#include "Jcy/CpuEncoder/VideoEncoder.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
JCY_WINDOWS_DISABLE_ALL_WARNING

namespace jcy
{
bool VideoEncoder::Initialize(int width, int height, int gopsize, int framerate)
{
  width_      = width;
  height_     = height;
  framerate_  = framerate;
  gopsize_    = gopsize;
  lplanesize_ = width * height;
  cplanesize_ = width * height / 4;
  uplaneos_   = width * height;
  vplaneos_   = width * height * 5 / 4;
  bs_.reserve(1000000);

  x264_param_default_preset(&param_, "medium", "zerolatency");
  param_.i_csp        = X264_CSP_I420;
  param_.i_width      = width;
  param_.i_height     = height;
  param_.i_bframe     = 0;
  param_.i_fps_num    = static_cast<uint32_t>(framerate);
  param_.i_fps_den    = 1;
  param_.b_vfr_input  = 0;
  param_.i_keyint_max = gopsize;
  // param_.i_frame_reference   = 4;
  param_.b_full_recon        = 1;
  param_.b_cabac             = 1;
  param_.b_deblocking_filter = 0;

  // param_.i_level_idc          = 51;
  param_.rc.i_rc_method   = X264_RC_CQP;
  param_.rc.i_qp_constant = 24;
  // param.rc.i_qp_min          = 24;
  // param.rc.i_qp_max          = 24;
  // param.rc.i_bitrate         = 1500;

  // Muxing params
  param_.b_annexb = 1;
  param_.b_aud    = 0;

  // Analyze
  param_.analyse.i_weighted_pred = 0;
  param_.analyse.b_fast_pskip    = 1;

  x264_param_apply_profile(&param_, "high");
  x264_picture_alloc(&picin_, X264_CSP_I420, width, height);

  encoder_ = x264_encoder_open(&param_);

  if (!encoder_)
  {
    std::cout << "Create encoder failed." << std::endl;
    return false;
  }

  return true;
}

void VideoEncoder::Destroy()
{
  x264_picture_clean(&picin_);

  if (encoder_)
  {
    x264_encoder_close(encoder_);
    encoder_ = nullptr;
  }
}

bool VideoEncoder::EncodeAFrame(const uint8_t* buffer)
{
  std::memcpy(picin_.img.plane[0], buffer, static_cast<size_t>(lplanesize_));
  std::memcpy(picin_.img.plane[1],
              buffer + uplaneos_,
              static_cast<size_t>(cplanesize_));
  std::memcpy(picin_.img.plane[2],
              buffer + vplaneos_,
              static_cast<size_t>(cplanesize_));

  int nalcnt   = 0;
  int nalbytes = 0;

  if ((framecnt_ % gopsize_) == 0)
  {
    picin_.i_type = X264_TYPE_I;
  }
  else
  {
    picin_.i_type = X264_TYPE_P;
  }

  nalbytes = x264_encoder_encode(encoder_, &nal_, &nalcnt, &picin_, &picout_);
  if (nalbytes < 0)
  {
    std::cout << "Encode frame failed on " << framecnt_ << std::endl;
    return false;
  }

  bs_.clear();

  for (int inal = 0; inal < nalcnt; inal++)
  {
    for (int b = 0; b < nal_[inal].i_payload; b++)
    {
      bs_.push_back(nal_[inal].p_payload[b]);
    }
  }

  framecnt_++;
  return true;
}

bool VideoEncoder::EncodeAFrame(const uint8_t* ybuffer,
                                const uint8_t* ubuffer,
                                const uint8_t* vbuffer)
{
  std::memcpy(picin_.img.plane[0], ybuffer, static_cast<size_t>(lplanesize_));
  std::memcpy(picin_.img.plane[1], ubuffer, static_cast<size_t>(cplanesize_));
  std::memcpy(picin_.img.plane[2], vbuffer, static_cast<size_t>(cplanesize_));

  int nalcnt   = 0;
  int nalbytes = 0;

  if ((framecnt_ % gopsize_) == 0)
  {
    picin_.i_type = X264_TYPE_I;
  }
  else
  {
    picin_.i_type = X264_TYPE_P;
  }

  nalbytes = x264_encoder_encode(encoder_, &nal_, &nalcnt, &picin_, &picout_);
  if (nalbytes < 0)
  {
    std::cout << "Encode frame failed on " << framecnt_ << std::endl;
    return false;
  }

  bs_.clear();

  for (int inal = 0; inal < nalcnt; inal++)
  {
    for (int b = 0; b < nal_[inal].i_payload; b++)
    {
      bs_.push_back(nal_[inal].p_payload[b]);
    }
  }

  framecnt_++;
  return true;
}

void VideoEncoder::GetBitStream(const uint8_t*& bitstream, size_t& bssize)
{
  bitstream = bs_.data();
  bssize    = bs_.size();
}

}  // namespace jcy
