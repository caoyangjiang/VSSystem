// Copyright 2017 Caoyang Jiang

#include "Jcy/DIP/CudaTools.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/core/cuda.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/photo/cuda.hpp"
#include "opencv2/xphoto.hpp"
JCY_WINDOWS_DISABLE_ALL_WARNING

namespace jcy
{
namespace cuda
{
__global__ static void AdjustContrastBrightnessKernel(
    uint8_t* data, int width, int height, double alpha, int beta)
{
  const int x = blockIdx.x * blockDim.x + threadIdx.x;
  const int y = blockIdx.y * blockDim.y + threadIdx.y;

  if ((x < width) && (y < height))
  {
    for (int c = 0; c < 3; c++)
    {
      uint8_t* channel = data + width * height * c;
      uint32_t pix     = *(channel + (x + y * width));

      pix                = alpha * pix + beta;
      // if (pix > 255) pix = 255;

      pix = 255 * (pix >= 255) + pix * (pix < 255);

      // if(x==0)
      // {
      //   std::printf("thread 0: %d\n",pix);
      // }
      *(channel + (x + y * width)) = static_cast<uint8_t>(pix);
    }
  }
}

void Tools::AdjustContrastBrightness(cv::Mat& img, double alpha, int beta)
{
  int blockdimx, blockdimy, griddimx, griddimy;

  blockdimx = 32;
  blockdimy = 16;
  griddimx  = (img.size().width + blockdimx - 1) / blockdimx;
  griddimy  = (img.size().height + blockdimy - 1) / blockdimy;

  dim3 blks(griddimx, griddimy);
  dim3 threads(blockdimx, blockdimy);

  uint8_t* dpix = nullptr;
  cudaMalloc(reinterpret_cast<void**>(&dpix),
             img.size().width * img.size().height * 3);
  cudaMemcpy(dpix,
             img.data,
             img.size().width * img.size().height * 3,
             cudaMemcpyHostToDevice);
  AdjustContrastBrightnessKernel<<<blks, threads>>>(
      dpix, img.size().width, img.size().height, alpha, beta);
  cudaDeviceSynchronize();
  cudaMemcpy(img.data,
             dpix,
             img.size().width * img.size().height * 3,
             cudaMemcpyDeviceToHost);
  cudaFree(dpix);
}

}  // namespace cuda
}  // namespace jcy
