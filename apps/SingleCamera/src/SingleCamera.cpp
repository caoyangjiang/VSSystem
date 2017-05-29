// Copyright 2017 Caoyang Jiang

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include "Jcy/Camera/Camera.h"

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
  std::ofstream testfile("test.yuv",
                         std::ios::out | std::ios::binary | std::ios::trunc);
  while (capcnt != 30)
  {
    testfile.write(reinterpret_cast<const char*>(cams[0].GetCurrFrame().data()),
                   cams[0].GetCurrFrame().size());
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
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
