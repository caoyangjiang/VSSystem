// Copyright @ 2017 Caoyang Jiang

#ifndef MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPPACKET_H_
#define MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPPACKET_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <iostream>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

#include "Jcy/RTPServer/Constants.h"

namespace jcy
{
struct FUAIndicator
{
  uint8_t type : 5;
  uint8_t nri : 2;
  uint8_t f : 1;
};

struct FUAHeader
{
  uint8_t type : 5;
  uint8_t r : 1;
  uint8_t e : 1;
  uint8_t s : 1;
};

class RTPHeader
{
 public:
  RTPHeader();
  ~RTPHeader();

  inline void SetVersion(uint32_t version)
  {
    version_ = version;
  }

  inline void SetPadding(uint32_t padding)
  {
    padding_ = padding;
  }

  inline void SetExtensionFlag(uint32_t extflag)
  {
    extflag_ = extflag;
  }

  inline void SetContribSourceCnt(uint32_t csrccnt)
  {
    csrccnt_ = csrccnt;
  }

  inline void SetMarker(uint32_t marker)
  {
    marker_ = marker;
  }

  inline void SetPayloadType(uint32_t payloadtype)
  {
    payloadtype_ = payloadtype;
  }

  inline void SetSequenceNumber(uint32_t seqnum)
  {
    seqnum_ = seqnum;
  }

  inline void SetTimeStamp(uint32_t ts)
  {
    ts_ = ts;
  }

  inline void SetSyncSource(uint32_t ssrc)
  {
    ssrc_ = ssrc;
  }

  inline uint32_t GetVersion() const
  {
    return version_;
  }

  inline uint32_t GetPadding() const
  {
    return padding_;
  }

  inline uint32_t GetExtensionFlag() const
  {
    return extflag_;
  }

  inline uint32_t GetContribSourceCnt() const
  {
    return csrccnt_;
  }

  inline uint32_t GetMarker() const
  {
    return marker_;
  }

  inline uint32_t GetPayloadType() const
  {
    return payloadtype_;
  }

  inline uint32_t GetSequenceNumber() const
  {
    return seqnum_;
  }

  inline uint32_t GetTimeStamp() const
  {
    return ts_;
  }

  inline uint32_t GetSyncSource() const
  {
    return ssrc_;
  }

  void SerializeBE(std::vector<uint8_t>& bytes) const;

  void SerializeLE(std::vector<uint8_t>& bytes) const;

  int GetSize() const;

  bool Check() const;

 private:
  uint32_t version_ = 2;
  uint32_t padding_;
  uint32_t extflag_ = 0;
  uint32_t csrccnt_;
  uint32_t marker_;
  uint32_t payloadtype_;
  uint32_t seqnum_;
  uint32_t ts_;
  uint32_t ssrc_;
  std::vector<uint32_t> csrclist_;
};

/**
 * @brief      Class for rtp packet.
 */
class RTPPacket
{
 public:
  RTPPacket();
  ~RTPPacket();

  inline void SetHeader(const RTPHeader& header)
  {
    header_ = header;
  }

  inline void SetHeader(RTPHeader&& header)
  {
    header_ = std::move(header);
  }

  inline void SetData(const std::vector<uint8_t>& data)
  {
    data_ = std::move(data);
  }

  inline void SetData(std::vector<uint8_t>&& data)
  {
    data_ = std::move(data);
  }

  void SetData(const uint8_t* buffer, size_t size);

  void SerializeBE(std::vector<uint8_t>& bytes) const;

  void SerializeLE(std::vector<uint8_t>& bytes) const;

  int GetSize() const;

  bool Check() const;

 private:
  RTPHeader header_;
  std::vector<uint8_t> data_;
};

}  // namespace jcy

#endif  // MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_RTPPACKET_H_
