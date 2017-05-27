// Copyright @ 2017 Caoyang Jiang

#include "Jcy/RTPServer/RTPH264Builder.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <cstring>
#include <iostream>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

#include "Jcy/RTPServer/Constants.h"
#include "Jcy/RTPServer/Nalu.h"

namespace jcy
{
RTPH264Builder::RTPH264Builder()
{
}

RTPH264Builder::~RTPH264Builder()
{
}

uint16_t RTPH264Builder::BuildSingle(const Nalu& nalu,
                                     uint32_t ts,
                                     uint16_t seqnum,
                                     RTPPacket& pkt)
{
  RTPHeader hdr;
  hdr.SetVersion(version_);
  hdr.SetPadding(padding_);
  hdr.SetExtensionFlag(extflag_);
  hdr.SetSyncSource(ssrc_);
  hdr.SetContribSourceCnt(0);
  hdr.SetMarker(0);
  hdr.SetPayloadType(payloadtype_);
  hdr.SetSequenceNumber(seqnum);
  hdr.SetTimeStamp(ts);

  pkt.SetHeader(hdr);
  pkt.SetData(nalu.GetData());
  seqnum++;
  return seqnum;
}

uint16_t RTPH264Builder::BuildFragment(const std::vector<Nalu>& frags,
                                       uint32_t ts,
                                       uint16_t seqnum,
                                       std::vector<RTPPacket>& pkts)
{
  FUAIndicator fuaidc;

  // Indicator do not change for fragments
  fuaidc.f    = 0;
  fuaidc.nri  = static_cast<uint8_t>(frags[0].GetNRI());
  fuaidc.type = FUA_FRAGEMENT_UNIT_A_TYPE;

  for (size_t f = 0; f < frags.size(); f++)
  {
    RTPHeader hdr;
    hdr.SetVersion(version_);
    hdr.SetPadding(padding_);
    hdr.SetExtensionFlag(extflag_);
    hdr.SetSyncSource(ssrc_);
    hdr.SetContribSourceCnt(0);
    hdr.SetMarker(0);
    hdr.SetPayloadType(payloadtype_);
    hdr.SetSequenceNumber(seqnum);
    hdr.SetTimeStamp(ts);

    FUAHeader fuahdr;
    fuahdr.r    = 0;
    fuahdr.type = static_cast<uint8_t>(frags[f].GetType());
    if (f == 0)
    {
      fuahdr.s = 1;
      fuahdr.e = 0;
    }
    else if (f == (frags.size() - 1))
    {
      fuahdr.s = 0;
      fuahdr.e = 1;
    }
    else
    {
      fuahdr.s = 0;
      fuahdr.e = 0;
    }

    // Prepare FU-A payload
    std::vector<uint8_t> pktdata(2 + frags[f].GetSize());
    pktdata[0] = *reinterpret_cast<uint8_t*>(&fuaidc);
    pktdata[1] = *reinterpret_cast<uint8_t*>(&fuahdr);
    std::memcpy(
        pktdata.data() + 2, frags[f].GetData().data(), frags[f].GetSize());

    RTPPacket pkt;
    pkt.SetHeader(hdr);
    pkt.SetData(std::move(pktdata));
    pkts.push_back(pkt);
    seqnum++;
  }

  return seqnum;
}

uint16_t RTPH264Builder::BuildEOF(uint32_t ts, uint16_t seqnum, RTPPacket& pkt)
{
  Nalu nalu(kAUDELIMITER,
            2,
            (kAUDELIMITER[0] >> 6) & 0x03,
            (kAUDELIMITER[0] & 0x1F),
            false,
            0);

  RTPHeader hdr;
  hdr.SetVersion(version_);
  hdr.SetPadding(padding_);
  hdr.SetExtensionFlag(extflag_);
  hdr.SetSyncSource(ssrc_);
  hdr.SetContribSourceCnt(0);
  hdr.SetMarker(1);
  hdr.SetPayloadType(payloadtype_);
  hdr.SetSequenceNumber(seqnum);
  hdr.SetTimeStamp(ts);

  pkt.SetHeader(hdr);
  pkt.SetData(nalu.GetData());
  seqnum++;
  return seqnum;
}

}  // namespace jcy
