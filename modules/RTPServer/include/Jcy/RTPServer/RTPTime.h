// Copyright @ 2017 Caoyang Jiang

#ifndef MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPTIME_H_
#define MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPTIME_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <iostream>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
/**
 * @brief      Class for rtp time. The accuracy is considered in microsecond
 *             level.
 */

class RTPTime
{
 public:
  enum EPOCHTYPE
  {
    NTP,
    UNIX
  };

 public:
  /**
   * @brief      Gets the current time.
   *
   * @return     The current time.
   */
  static RTPTime GetCurTime();

 public:
  RTPTime();
  explicit RTPTime(uint64_t ntpuscnt);
  RTPTime(uint64_t uscnt, EPOCHTYPE type);
  ~RTPTime();

  inline RTPTime &operator-=(const RTPTime &t)
  {
    uscnt_ -= t.uscnt_;
    return *this;
  }

  inline RTPTime &operator+=(const RTPTime &t)
  {
    uscnt_ += t.uscnt_;
    return *this;
  }

  inline RTPTime &operator=(const RTPTime &t)
  {
    uscnt_ = t.uscnt_;
    return *this;
  }

  /**
   * @brief      Gets the seconds in double format.
   *
   * @return     The seconds in double.
   */
  inline double GetSecInDouble() const
  {
    return static_cast<double>(uscnt_) / 1000000.0f;
  }

  /**
   * @brief      Gets the milli second in double fromat.
   *
   * @return     The milliseconds in double.
   */
  inline double GetMilliSecInDouble() const
  {
    return static_cast<double>(uscnt_) / 1000.0f;
  }

  /**
   * @brief      Gets the counts in microseconds.
   *
   * @return     The counts.
   */
  inline uint64_t GetCounts() const
  {
    return uscnt_;
  }

  /**
   * @brief      Gets the ntp time in 32-bit format.
   *
   * @return     The ntp 32 bit format.
   */
  uint32_t GetNTP32BitFmt() const;

  /**
   * @brief      Gets the ntp time in 64-bit format.
   *
   * @return     The ntp 64 bit format.
   */
  uint64_t GetNTP64BitFmt() const;

 private:
  inline uint32_t GetSecInSecCnt() const
  {
    return static_cast<uint32_t>(uscnt_ / 1000000);
  }

  inline uint32_t GetFracSecInMicroSecCnt() const
  {
    return static_cast<uint32_t>(uscnt_ - (uscnt_ / 1000000) * 1000000);
  }

 private:
  uint64_t uscnt_ = 0;
};

inline RTPTime operator-(const RTPTime &t1, const RTPTime &t2)
{
  return RTPTime(t1.GetCounts() - t2.GetCounts());
}

inline RTPTime operator+(const RTPTime &t1, const RTPTime &t2)
{
  return RTPTime(t1.GetCounts() + t2.GetCounts());
}

inline bool operator>(const RTPTime &t1, const RTPTime &t2)
{
  if (t1.GetCounts() > t2.GetCounts())
    return true;
  else
    return false;
}

inline bool operator<(const RTPTime &t1, const RTPTime &t2)
{
  if (t1.GetCounts() < t2.GetCounts())
    return true;
  else
    return false;
}

inline bool operator>=(const RTPTime &t1, const RTPTime &t2)
{
  if (t1.GetCounts() >= t2.GetCounts())
    return true;
  else
    return false;
}

inline bool operator<=(const RTPTime &t1, const RTPTime &t2)
{
  if (t1.GetCounts() <= t2.GetCounts())
    return true;
  else
    return false;
}

inline bool operator==(const RTPTime &t1, const RTPTime &t2)
{
  if (t1.GetCounts() == t2.GetCounts())
    return true;
  else
    return false;
}

}  // namespace jcy

#endif  // MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPTIME_H_
