// Copyright 2017 Caoyang Jiang

#include "Jcy/Camera/MultiCamera.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include "Jcy/CpuEncoder/VideoEncoder.h"
#include "Jcy/DIP/Tools.h"
#include "Jcy/GpuEncoder/GpuVideoEncoder.h"
#include "Jcy/RTPServer/RTPSession.h"
#include "boost/asio.hpp"
#include "opencv2/opencv.hpp"

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

  jcy::MultiCamera cams;
  std::vector<std::string> devices;
  devices.push_back("/dev/video0");
  devices.push_back("/dev/video1");
  cams.Initialize(devices, targetframerate, targetwidth, targetheight);

  int capcnt      = 0;
  int rendercamid = 0;

  // jcy::VideoEncoder encoder;
  jcy::GpuVideoEncoder encoder;
  jcy::RTPSession rtpsession(targetframerate);

  if (!rtpsession.Initialize()) return -1;
  if (!encoder.Initialize(targetwidth, targetheight, 3, targetframerate))
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

  cams.StartCapture();
  while (true)
  {
    std::chrono::duration<double, std::milli> dur;
    std::chrono::high_resolution_clock::time_point beg, end;
    size_t bssize       = 0;
    const uint8_t* bits = nullptr;

    beg = std::chrono::high_resolution_clock::now();

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
    jcy::Tools::AdjustContrastBrightness(merged, 1.2, 15);

    cv::namedWindow("Test", CV_WINDOW_AUTOSIZE);
    cv::imshow("Test", merged);
    cv::waitKey(10);
    cv::Mat yuvimg;
    cv::Mat img(
        targetheight,
        targetwidth,
        CV_8UC3,
        const_cast<unsigned char*>(
            cams.GetCurrFrame(cams.GetDeviceIDs()[rendercamid]).data()));

    cv::cvtColor(img, yuvimg, cv::COLOR_BGR2YUV_I420);
    encoder.EncodeAFrame(yuvimg.data);
    encoder.GetBitStream(bits, bssize);

    end = std::chrono::high_resolution_clock::now();
    dur = end - beg;

    int durms = dur.count();
    int sleepms =
        durms >= 1000 / targetframerate ? 0 : (1000 / targetframerate - durms);

    if (sleepms != 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepms));

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

    capcnt++;
    // if ((capcnt % 900) == 0) rendercamid = rendercamid == 0 ? 1 : 0;
  }

  socket.close();
  cams.StopCapture();
  cams.Destroy();

  return 0;
}
