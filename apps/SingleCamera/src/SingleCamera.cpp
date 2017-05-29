// Copyright 2017 Caoyang Jiang

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include "Jcy/Camera/Camera.h"
#include "Jcy/CpuEncoder/VideoEncoder.h"

int main()
{
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
    if (!cams[devid].Initialize(devices[devid], 60, 800, 600))
    {
      return -1;
    }
  }

  cams[0].StartCapture();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  int capcnt = 0;
  jcy::VideoEncoder encoder;
  encoder.Initialize(800, 600, 1, 60);

  std::ofstream testfile("test.h264",
                         std::ios::out | std::ios::binary | std::ios::trunc);
  while (capcnt != 300)
  {
    size_t bssize       = 0;
    const uint8_t* bits = nullptr;
    encoder.EncodeAFrame(cams[0].GetCurrFrame().data());
    encoder.GetBitStream(bits, bssize);
    testfile.write(reinterpret_cast<const char*>(bits),
                   static_cast<std::streamsize>(bssize));
    // testfile.write(reinterpret_cast<const
    // char*>(cams[0].GetCurrFrame().data()),
    //                cams[0].GetCurrFrame().size());
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
    capcnt++;
  }

  testfile.close();
  cams[0].StopCapture();

  for (size_t devid = 0; devid < cams.size(); devid++)
  {
    cams[devid].Destroy();
  }

  jcy::Camera::DestroyDeviceList();
  jcy::Camera::DestroyUVCContext();
  return 0;
}
