// Copyright @ 2017 Caoyang Jiang

#include "Jcy/RTPServer/RTPSession.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

#include "Jcy/RTPServer/Constants.h"
#include "Jcy/RTPServer/RTPH264Builder.h"

namespace jcy
{
RTPSession::RTPSession(uint32_t framerate)
{
  std::default_random_engine generator;
  std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());
  std::chrono::high_resolution_clock::time_point time =
      std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::duration dtn = time.time_since_epoch();
  generator.seed(static_cast<uint32_t>(dtn.count()));

  // Generate random ssrc and sequence number
  clock_     = RTP_H264_CLOCK_RATE;
  seqnum_    = static_cast<uint16_t>(dist(generator));
  ssrc_      = static_cast<uint32_t>(dist(generator));
  framerate_ = framerate;
  ts_        = static_cast<uint32_t>(dist(generator));
  inc_       = k90HZCLOCK_ / framerate;
}

RTPSession::~RTPSession()
{
}

bool RTPSession::Initialize()
{
  pktbuilder_ = std::make_shared<RTPH264Builder>();

  // Set option that do not vary with packetize
  pktbuilder_->SetPayLoadType(RTP_H264_PAYLOAD_TYPE_ID);
  pktbuilder_->SetVersion(RTP_VERSION_ID);
  pktbuilder_->SetPaddingBit(0);
  pktbuilder_->SetExtensionFlag(0);
  pktbuilder_->SetSyncSource(ssrc_);

  return true;
}

bool RTPSession::Initialize(const RTPSessionParam& param)
{
  if (param.useextclock) clock_   = param.clock;
  if (param.useextseqnum) seqnum_ = param.seqnum;
  if (param.useextssrc) ssrc_     = param.ssrc;

  return Initialize();
}

bool RTPSession::Packetize(const uint8_t* bitstream,
                           size_t size,
                           std::vector<RTPPacket>& pkts)
{
  prev_ = curr_;
  curr_ = RTPTime::GetCurTime();

  // Convert NTP absolute time into h264 timestamp measured in ticks from a 90
  // KHz clock. This is possible because on the timestamp difference matters
  // uint32_t ts = static_cast<uint32_t>(curr_.GetCounts() * clock_ / 1000000);

  // If input bitstream has multiple nalus, they are split
  std::vector<Nalu> nalus;
  NaluTools::SplitNaluStream(bitstream, size, nalus);

  for (size_t c = 0; c < nalus.size(); c++)
  {
    if (nalus[c].GetSize() > MTU_ADJUSTED_RTP_MAXIMUM_PAYLOAD_SIZE)
    {
      std::vector<Nalu> frags;
      nalus[c].Fragment(MTU_ADJUSTED_RTP_MAXIMUM_PAYLOAD_SIZE, frags);
      seqnum_ = pktbuilder_->BuildFragment(frags, ts_, seqnum_, pkts);
    }
    else
    {
      RTPPacket pkt;
      seqnum_ = pktbuilder_->BuildSingle(nalus[c], ts_, seqnum_, pkt);
      pkts.push_back(pkt);
    }
  }

  ts_ += inc_;
  return true;
}

bool RTPSession::Packetize(const uint8_t* bitstream,
                           size_t size,
                           std::vector<std::vector<uint8_t>>& pktsbytes)
{
  std::vector<RTPPacket> pkts;

  if (!Packetize(bitstream, size, pkts))
  {
    return false;
  }

  for (auto pkt : pkts)
  {
    std::vector<uint8_t> bytes;
    pkt.SerializeBE(bytes);
    pktsbytes.push_back(bytes);
  }

  return true;
}

bool RTPSession::GetByePacket(RTPPacket& pkt)
{
  prev_       = curr_;
  curr_       = RTPTime::GetCurTime();
  uint32_t ts = static_cast<uint32_t>(curr_.GetCounts() * clock_ / 1000000);
  seqnum_     = pktbuilder_->BuildEOF(ts, seqnum_, pkt);
  return true;
}

bool RTPSession::GetByePacket(std::vector<uint8_t>& pktbytes)
{
  RTPPacket pkt;

  if (!GetByePacket(pkt))
  {
    return false;
  }

  pkt.SerializeBE(pktbytes);
  return true;
}

}  // namespace jcy
