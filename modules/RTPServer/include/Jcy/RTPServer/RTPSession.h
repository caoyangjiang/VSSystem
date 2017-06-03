// Copyright @ 2017 Caoyang Jiang

#ifndef MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPSESSION_H_
#define MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPSESSION_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <iostream>
#include <memory>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

#include "Jcy/RTPServer/RTPPacket.h"
#include "Jcy/RTPServer/RTPTime.h"

namespace jcy
{
class RTPH264Builder;

struct RTPSessionParam
{
  bool useextclock  = false;
  bool useextseqnum = false;
  bool useextssrc   = false;
  uint32_t clock    = 0;
  uint16_t seqnum   = 0;
  uint32_t ssrc     = 0;
};

/**
 * @brief      Class for rtp session. (NOT thread-safe yet)
 */
class RTPSession
{
 public:
  explicit RTPSession(uint32_t framerate);
  ~RTPSession();

  /**
   * @brief      Initialize with default parameters.
   *
   * @return     True if successful, false otherwise.
   */
  bool Initialize();

  /**
   * @brief      Intialize Session with external paramters.
   *
   * @param[in]  param  The parameter
   *
   * @return     True if successful, false otherwise.
   */
  bool Initialize(const RTPSessionParam& param);

  /**
   * @brief      Packetize into packets
   *
   * @param[in]  bitstream  The bitstream
   * @param[in]  size       The size
   * @param      pkts       The packets
   *
   * @return     True if successful, false otherwise.
   */
  bool Packetize(const uint8_t* bitstream,
                 size_t size,
                 std::vector<RTPPacket>& pkts);

  /**
   * @brief      Packetize into packet byte stream
   *
   * @param[in]  bitstream  The bitstream
   * @param[in]  size       The size
   * @param      pktsbytes  The packets
   *
   * @return     True if successful, false otherwise.
   */
  bool Packetize(const uint8_t* bitstream,
                 size_t size,
                 std::vector<std::vector<uint8_t>>& pktsbytes);

  /**
   * @brief      Gets the bye packet.
   *
   * @param      pktbytes  The packets
   *
   * @return     The bye packet.
   */
  bool GetByePacket(std::vector<uint8_t>& pktbytes);

  /**
   * @brief      Gets the bye packet.
   *
   * @param      pkt   The packet
   *
   * @return     The bye packet.
   */
  bool GetByePacket(RTPPacket& pkt);

  /**
   * @brief      Gets the current time stamp.
   *
   * @return     The current time stamp.
   */
  inline RTPTime GetCurrTimeStamp() const
  {
    return curr_;
  }

  /**
   * @brief      Gets the previous time stamp.
   *
   * @return     The previous time stamp.
   */
  inline RTPTime GetPrevTimeStamp() const
  {
    return prev_;
  }

  /**
   * @brief      Gets the clock rate.
   *
   * @return     The clock rate.
   */
  inline uint32_t GetClockRate() const
  {
    return clock_;
  }

  /**
   * @brief      Gets the current sequence number.
   *
   * @return     The sequence number.
   */
  inline uint32_t GetSequenceNum() const
  {
    return seqnum_;
  }

  /**
   * @brief      Gets the ssrc identifier.
   *
   * @return     The ssrc.
   */
  inline uint32_t GetSSRC() const
  {
    return ssrc_;
  }

 private:
  uint32_t clock_     = 0;
  uint16_t seqnum_    = 0;
  uint32_t ssrc_      = 0;
  uint32_t ts_        = 0;
  uint32_t inc_       = 0;
  uint32_t framerate_ = 0;
  RTPTime curr_;
  RTPTime prev_;
  const uint32_t k90HZCLOCK_ = 90000;
  std::shared_ptr<RTPH264Builder> pktbuilder_;
};

}  // namespace jcy

#endif  // MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPSESSION_H_
