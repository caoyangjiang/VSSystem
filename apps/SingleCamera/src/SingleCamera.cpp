// Copyright 2017 Caoyang Jiang

#include <chrono>
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
    if (!cams[devid].Initialize(devices[devid], 30, 1920, 1080))
    {
      return -1;
    }
  }

  cams[0].StartCapture();
  std::this_thread::sleep_for(std::chrono::seconds(100));
  cams[0].StopCapture();

  for (auto cam : cams)
  {
    cam.Destroy();
  }

  jcy::Camera::DestroyDeviceList();
  jcy::Camera::DestroyUVCContext();
  return 0;
}
