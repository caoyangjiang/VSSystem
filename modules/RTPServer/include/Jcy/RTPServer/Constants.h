// Copyright @ 2017 Caoyang Jiang

#ifndef MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_CONSTANTS_H_
#define MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_CONSTANTS_H_

namespace jcy
{
// Time from 01/01/1900 to 01/01/1970
#define NTP_TO_UNIX_EPOCH_GAP_USEC 2208988800000000ul
#define NTP_TO_UNIX_EPOCH_GAP_MSEC 2208988800000ul
#define NTP_TO_UNIX_EPOCH_GAP_SEC 2208988800ul

// IP Packet size
#define IP_PACKET_SIZE 65535

// IP header size
#define IP_HEADER_SIZE 20

// IP packet maximum payload size
#define IP_MAXIMUM_PAYLOAD_SIZE 65515

// UDP packet header size
#define UDP_HEADER_SIZE 8

// UDP packet maximum payload size
#define UDP_MAXIMUM_PAYLOAD_SIZE 65507

// RTP packet version number
#define RTP_VERSION_ID 2

// RTP payload type for H264
#define RTP_H264_PAYLOAD_TYPE_ID 96

// RTP clock rate for H264
#define RTP_H264_CLOCK_RATE 90000

// Excluding CSRC List
#define RTP_HEADER_FIX_PORTION_SIZE 12

#define RTP_CSRC_SIZE 4

// This the maximum payload without csrc
#define RTP_MAXIMUM_PAYLOAD_SIZE 65495

// Ethernet packet size
#define MTU_PAYLOAD_SIZE 1500

// Fragmented NALU type inside RTP payload
#define FUA_FRAGEMENT_UNIT_A_TYPE 28

// Maximum UDP payload size to fit in one MTU packet
#define MTU_ADJUSTED_UDP_MAXIMUM_PAYLOAD_SIZE 1472

// This the maximum payload without csrc
#define MTU_ADJUSTED_RTP_MAXIMUM_PAYLOAD_SIZE 1460

}  // namespace jcy

#endif  // MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_CONSTANTS_H_
