// Copyright 2017 Caoyang Jiang

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include "Jcy/Camera/Camera.h"
#include "Jcy/CpuEncoder/VideoEncoder.h"
#include "Jcy/GpuEncoder/GpuVideoEncoder.h"
#include "Jcy/RTPServer/RTPSession.h"
#include "boost/asio.hpp"

int targetwidth     = 1024;
int targetheight    = 768;
int targetframerate = 30;

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "RTPServer DEST_IP" << std::endl;
    return false;
  }

  if (!jcy::Camera::InitializeUVCContext())
  {
    return -1;
  }

  if (!jcy::Camera::CreateDeviceList())
  {
    jcy::Camera::DestroyUVCContext();
    return -1;
  }

  std::vector<uvc_device_t*> devices = jcy::Camera::GetDevicesList();

  std::cout << "[Message]: Found " << devices.size() << " Devices. "
            << std::endl;

  std::vector<jcy::Camera> cams(devices.size());

  for (size_t devid = 0; devid < devices.size(); devid++)
  {
    if (!cams[devid].Initialize(
            devices[devid], targetframerate, targetwidth, targetheight))
    {
      return -1;
    }
  }

  cams[0].StartCapture();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  int capcnt = 0;
  // jcy::VideoEncoder encoder;
  jcy::GpuVideoEncoder encoder;
  jcy::RTPSession rtpsession;

  if (!rtpsession.Initialize()) return -1;
  if (!encoder.Initialize(targetwidth, targetheight, 1, targetframerate))
    return -1;
  boost::asio::io_service io_service;
  boost::asio::ip::udp::socket socket(
      io_service,
      boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
  boost::asio::ip::udp::resolver resolver(io_service);
  boost::asio::ip::udp::resolver::query query(
      boost::asio::ip::udp::v4(), argv[1], "5004");
  boost::asio::ip::udp::resolver::iterator it   = resolver.resolve(query);
  boost::asio::ip::udp::endpoint remoteendpoint = *it;

  // socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
  // socket.set_option(boost::asio::socket_base::broadcast(true));
  // boost::asio::ip::udp::endpoint remoteendpoint(
  //     boost::asio::ip::address_v4::broadcast(), 5004);

  std::ofstream testfile("test.h264",
                         std::ios::out | std::ios::binary | std::ios::trunc);
  // while (capcnt != 300)
  while (true)
  {
    std::chrono::duration<double, std::milli> dur;
    std::chrono::high_resolution_clock::time_point beg, end;
    size_t bssize       = 0;
    const uint8_t* bits = nullptr;

    beg = std::chrono::high_resolution_clock::now();
    encoder.EncodeAFrame(cams[0].GetCurrFrame().data());
    encoder.GetBitStream(bits, bssize);

    // Write encoded
    // testfile.write(reinterpret_cast<const char*>(bits),
    //                static_cast<std::streamsize>(bssize));

    // Write raw
    // testfile.write(reinterpret_cast<const
    // char*>(cams[0].GetCurrFrame().data()),
    //                cams[0].GetCurrFrame().size());

    std::vector<std::vector<uint8_t>> pktsbytes;

    if (!rtpsession.Packetize(bits, bssize, pktsbytes))
    {
      std::cout << "[Error]: packetize failed" << std::endl;
      break;
    }
    else
    {
      for (size_t p = 0; p < pktsbytes.size(); p++)
      {
        socket.send_to(
            boost::asio::buffer(pktsbytes[p].data(), pktsbytes[p].size()),
            remoteendpoint);
      }
    }
    end = std::chrono::high_resolution_clock::now();
    dur = end - beg;

    int durms = dur.count();
    int sleepms =
        durms >= 1000 / targetframerate ? 0 : (1000 / targetframerate - durms);

    if (sleepms != 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));

    // std::cout << "Sleep for " << sleepms << " ms."<< std::endl;
    capcnt++;
  }

  testfile.close();
  socket.close();
  cams[0].StopCapture();

  encoder.Destroy();
  for (size_t devid = 0; devid < cams.size(); devid++)
  {
    cams[devid].Destroy();
  }

  jcy::Camera::DestroyDeviceList();
  jcy::Camera::DestroyUVCContext();
  return 0;
}
