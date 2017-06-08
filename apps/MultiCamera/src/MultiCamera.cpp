// Copyright 2017 Caoyang Jiang

#include "Jcy/Camera/MultiCamera.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include "Jcy/CpuEncoder/VideoEncoder.h"
#include "Jcy/DIP/CudaTools.h"
#include "Jcy/DIP/Tools.h"

#include "Jcy/GpuEncoder/GpuVideoEncoder.h"
#include "Jcy/RTPServer/RTPSession.h"
#include "boost/asio.hpp"
#include "opencv2/opencv.hpp"

int targetwidth     = 1024;
int targetheight    = 768;
int targetframerate = 30;

int main(int argc, char**)
{
  if (argc < 1)
  {
    std::cout << "RTPServer DEST_IP" << std::endl;
    return false;
  }

  jcy::MultiCamera cams;
  std::vector<std::string> devices;
  devices.push_back("/dev/video0");
  devices.push_back("/dev/video1");
  cams.Initialize(devices, targetframerate, targetwidth, targetheight);

  int capcnt = 0;
  // int rendercamid = 0;

  // jcy::VideoEncoder encoder;
  jcy::GpuVideoEncoder encoder;
  jcy::RTPSession rtpsession(targetframerate);

  if (!rtpsession.Initialize()) return -1;
  if (!encoder.Initialize(targetwidth * 2, targetheight, 3, targetframerate))
    return -1;

  std::vector<std::string> hosts;
  hosts.push_back("10.8.0.10");
  // hosts.push_back("127.0.0.1");
  // hosts.push_back("192.168.1.4");

  boost::asio::io_service io_service;
  boost::asio::ip::udp::socket socket(
      io_service,
      boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
  boost::asio::ip::udp::resolver resolver(io_service);
  std::vector<boost::asio::ip::udp::endpoint> endpoints;

  for (const auto& host : hosts)
  {
    boost::asio::ip::udp::resolver::query query(
        boost::asio::ip::udp::v4(), host, "5004");
    boost::asio::ip::udp::resolver::iterator it   = resolver.resolve(query);
    boost::asio::ip::udp::endpoint remoteendpoint = *it;
    endpoints.push_back(remoteendpoint);
  }

  cams.StartCapture();
  while (capcnt < 900)
  //while (true)
  {
    std::chrono::duration<double, std::milli> dur;
    std::chrono::high_resolution_clock::time_point beg, end;

    beg = std::chrono::high_resolution_clock::now();

    // Prepare for merging
    std::vector<cv::Mat> imgs(cams.GetDeviceIDs().size());
    for (size_t i = 0; i < cams.GetDeviceIDs().size(); i++)
    {
      imgs[i] = cv::Mat(targetheight,
                        targetwidth,
                        CV_8UC3,
                        const_cast<unsigned char*>(
                            cams.GetCurrFrame(cams.GetDeviceIDs()[i]).data()));
    }

    cv::Mat merged;
    jcy::Tools::Merge(imgs, 2, merged);
    jcy::cuda::Tools::AdjustContrastBrightness(merged, 1.2, 20);
    // jcy::Tools::ShowImage(merged, "Test");
    
    std::string name = "./Save/Image" + std::to_string(capcnt) + ".jpg";
    cv::imwrite(name, imgs[0]);
    
    // jcy::Tools::ShowImage(imgs[1], "Test");
    // Convert to yuv
    cv::Mat yuvimg;
    cv::cvtColor(merged, yuvimg, cv::COLOR_BGR2YUV_I420);

    // Encode
    size_t bssize       = 0;
    const uint8_t* bits = nullptr;
    encoder.EncodeAFrame(yuvimg.data);
    encoder.GetBitStream(bits, bssize);

    // RTP packing
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
        for (auto& endpoint : endpoints)
        {
          socket.send_to(
              boost::asio::buffer(pktsbytes[p].data(), pktsbytes[p].size()),
              endpoint);
        }
      }
    }

    end = std::chrono::high_resolution_clock::now();
    dur = end - beg;

    int durms = dur.count();
    int sleepms =
        durms >= 1000 / targetframerate ? 0 : (1000 / targetframerate - durms);

    if (sleepms != 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));

    // std::cout << "Time left: " << sleepms << std::endl;

    capcnt++;
  }

  socket.close();
  cams.StopCapture();
  cams.Destroy();

  return 0;
}
