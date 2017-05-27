// Copyright @ 2017 Caoyang Jiang

#include "Jcy/RTPServer/RTPTime.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <chrono>
JCY_WINDOWS_ENABLE_ALL_WARNING

#include "Jcy/RTPServer/Constants.h"

namespace jcy
{
RTPTime RTPTime::GetCurTime()
{
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
  std::chrono::system_clock::duration dtn  = tp.time_since_epoch();
  uint64_t micro =
      std::chrono::duration_cast<std::chrono::microseconds>(dtn).count();
  return RTPTime(micro, EPOCHTYPE::UNIX);
}

RTPTime::RTPTime()
{
}

RTPTime::RTPTime(uint64_t ntpuscnt)
{
  uscnt_ = ntpuscnt;
}

RTPTime::RTPTime(uint64_t uscnt, EPOCHTYPE type)
{
  if (type == NTP)
  {
    uscnt_ = uscnt;
  }
  else
  {
    uscnt_ = uscnt + NTP_TO_UNIX_EPOCH_GAP_USEC;
  }
}

RTPTime::~RTPTime()
{
}

uint32_t RTPTime::GetNTP32BitFmt() const
{
  uint64_t ntp64 = GetNTP64BitFmt();
  uint32_t ntp32 = static_cast<uint32_t>(ntp64 >> 16);
  return ntp32;
}

uint64_t RTPTime::GetNTP64BitFmt() const
{
  uint64_t intsec = 0, fracsec = 0;
  intsec  = GetSecInSecCnt();
  fracsec = GetFracSecInMicroSecCnt();
  fracsec = fracsec * (1ul << 32) / 1000000;
  return intsec << 32 | fracsec;
}

}  // namespace jcy
