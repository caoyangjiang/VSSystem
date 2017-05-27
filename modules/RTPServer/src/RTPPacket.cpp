// Copyright @ 2017 Caoyang Jiang

#include "Jcy/RTPServer/RTPPacket.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <iostream>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

#include "Jcy/RTPServer/Constants.h"

namespace jcy
{
RTPHeader::RTPHeader()
{
}

RTPHeader::~RTPHeader()
{
}

void RTPHeader::SerializeBE(std::vector<uint8_t>& bytes) const
{
  struct TmpPartHdr
  {
    uint32_t seq : 16;
    uint32_t pt : 7;
    uint32_t m : 1;
    uint32_t cc : 4;
    uint32_t x : 1;
    uint32_t p : 1;
    uint32_t version : 2;
  };

  TmpPartHdr parthdr;
  parthdr.version = version_;
  parthdr.p       = padding_;
  parthdr.x       = extflag_;
  parthdr.cc      = csrccnt_;
  parthdr.m       = marker_;
  parthdr.pt      = payloadtype_;
  parthdr.seq     = seqnum_;

  // LE to BE
  for (int i = 3; i >= 0; i--)
  {
    bytes.push_back(*(reinterpret_cast<uint8_t*>(&parthdr) + i));
  }

  uint8_t b = 0;

  b = static_cast<uint8_t>((ts_ >> 24) & 0xFF);
  bytes.push_back(b);

  b = static_cast<uint8_t>((ts_ >> 16) & 0xFF);
  bytes.push_back(b);

  b = static_cast<uint8_t>((ts_ >> 8) & 0xFF);
  bytes.push_back(b);

  b = static_cast<uint8_t>(ts_ & 0xFF);
  bytes.push_back(b);

  b = static_cast<uint8_t>((ssrc_ >> 24) & 0xFF);
  bytes.push_back(b);

  b = static_cast<uint8_t>((ssrc_ >> 16) & 0xFF);
  bytes.push_back(b);

  b = static_cast<uint8_t>((ssrc_ >> 8) & 0xFF);
  bytes.push_back(b);

  b = static_cast<uint8_t>(ssrc_ & 0xFF);
  bytes.push_back(b);

  if (csrccnt_ > 0)
  {
    for (size_t i = 0; i < csrccnt_; i++)
    {
      uint32_t csrc = csrclist_[i];

      b = static_cast<uint8_t>((csrc >> 24) & 0xFF);
      bytes.push_back(b);

      b = static_cast<uint8_t>((csrc >> 16) & 0xFF);
      bytes.push_back(b);

      b = static_cast<uint8_t>((csrc >> 8) & 0xFF);
      bytes.push_back(b);

      b = static_cast<uint8_t>(csrc & 0xFF);
      bytes.push_back(b);
    }
  }
}

void RTPHeader::SerializeLE(std::vector<uint8_t>& bytes) const
{
  std::vector<uint8_t> be;

  bytes.clear();

  SerializeBE(be);

  for (std::vector<uint8_t>::reverse_iterator it = be.rbegin(); it != be.rend();
       it++)
  {
    bytes.push_back(*it);
  }
}

int RTPHeader::GetSize() const
{
  return static_cast<int>(RTP_HEADER_FIX_PORTION_SIZE +
                          RTP_CSRC_SIZE * GetContribSourceCnt());
}

bool RTPHeader::Check() const
{
  if (version_ != 2)
  {
    std::cout << "[ERROR]: Version not 2" << std::endl;
    return false;
  }

  if (csrccnt_ != csrclist_.size())
  {
    std::cout << "[ERROR]: Contributing source count incorrect." << std::endl;
    return false;
  }

  if (seqnum_ > ((1u << 16) - 1))
  {
    std::cout << "[ERROR]: sequence number greater than 2^16 - 1." << std::endl;
    return false;
  }

  return true;
}

RTPPacket::RTPPacket()
{
}

RTPPacket::~RTPPacket()
{
}

void RTPPacket::SetData(const uint8_t* buffer, size_t size)
{
  data_.clear();
  for (size_t i = 0; i < size; i++) data_.push_back(buffer[i]);
}

void RTPPacket::SerializeBE(std::vector<uint8_t>& bytes) const
{
  std::vector<uint8_t> hdrbytes;
  header_.SerializeBE(hdrbytes);

  for (auto b : data_)
  {
    hdrbytes.push_back(b);
  }

  bytes = std::move(hdrbytes);
}

void RTPPacket::SerializeLE(std::vector<uint8_t>& bytes) const
{
  std::vector<uint8_t> hdrbytes;
  header_.SerializeLE(hdrbytes);

  for (auto b : data_)
  {
    hdrbytes.push_back(b);
  }

  bytes = std::move(hdrbytes);
}

int RTPPacket::GetSize() const
{
  return static_cast<int>(data_.size()) + header_.GetSize();
}

bool RTPPacket::Check() const
{
  if (!header_.Check())
  {
    std::cout << "[ERROR]: RTP header error." << std::endl;
    return false;
  }

  if ((data_.size() + header_.GetSize()) > UDP_MAXIMUM_PAYLOAD_SIZE)
  {
    std::cout
        << "[ERROR]: RTP packet payload size greater than UDP maximum size "
        << std::endl;
    return false;
  }

  if (header_.GetExtensionFlag() != 0)
  {
    std::cout << "[ERROR]: RTP packet extension not supported." << std::endl;
    return false;
  }

  return true;
}

}  // namespace jcy
