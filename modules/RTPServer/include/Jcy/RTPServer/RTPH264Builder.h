// Copyright @ 2017 Caoyang Jiang

#ifndef MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPH264BUILDER_H_
#define MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPH264BUILDER_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <iostream>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

#include "Jcy/RTPServer/Nalu.h"
#include "Jcy/RTPServer/RTPPacket.h"

namespace jcy
{
class RTPH264Builder
{
 public:
  RTPH264Builder();
  ~RTPH264Builder();

  /**
   * @brief      Builds a single.
   *
   * @param[in]  nalu    The nalu
   * @param[in]  ts      The time stamp
   * @param[in]  seqnum  The seqnum
   * @param[out] pkt     The packet
   *
   * @return     The incremented seqnum.
   */
  uint16_t BuildSingle(const Nalu& nalu,
                       uint32_t ts,
                       uint16_t seqnum,
                       RTPPacket& pkt);

  /**
   * @brief      Builds packets from nalu fragments.
   *
   * @param[in]  frags   The frags
   * @param[in]  ts      The time stamp
   * @param[in]  seqnum  The seqnum
   * @param[out] pkts    The packets
   *
   * @return     The incremented seqnum.
   */
  uint16_t BuildFragment(const std::vector<Nalu>& frags,
                         uint32_t ts,
                         uint16_t seqnum,
                         std::vector<RTPPacket>& pkts);

  /**
   * @brief      Builds a last.
   *
   * @param[in]  ts      The timestamp
   * @param[in]  seqnum  The seqnum
   * @param[out] pkt     The packets
   *
   * @return     The incremented seqnum.
   */
  uint16_t BuildEOF(uint32_t ts, uint16_t seqnum, RTPPacket& pkt);

  /**
   * @brief      Sets the pay load type.
   *
   * @param[in]  payloadtype  The payloadtype
   */
  inline void SetPayLoadType(uint32_t payloadtype)
  {
    payloadtype_ = payloadtype;
  }

  /**
   * @brief      Sets the version.
   *
   * @param[in]  version  The version
   */
  inline void SetVersion(uint32_t version)
  {
    version_ = version;
  }

  /**
   * @brief      Sets the padding bit.
   *
   * @param[in]  padding  The padding
   */
  inline void SetPaddingBit(uint32_t padding)
  {
    padding_ = padding;
  }

  /**
   * @brief      Sets the extension flag.
   *
   * @param[in]  extflag  The extflag
   */
  inline void SetExtensionFlag(uint32_t extflag)
  {
    extflag_ = extflag;
  }

  /**
   * @brief      Sets the synchronize source.
   *
   * @param[in]  ssrc  The ssrc
   */
  inline void SetSyncSource(uint32_t ssrc)
  {
    ssrc_ = ssrc;
  }

 private:
  uint32_t version_             = 2;
  uint32_t padding_             = 0;
  uint32_t extflag_             = 0;
  uint32_t payloadtype_         = 96;
  uint32_t ssrc_                = 0;
  const uint8_t kAUDELIMITER[2] = {0x09, 00};
  const size_t kAUDELIMITERSIZE = 2;
};

}  // namespace hvr
#endif  // MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPH264BUILDER_H_
