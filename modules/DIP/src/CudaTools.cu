// Copyright 2017 Caoyang Jiang

#include "Jcy/DIP/CudaTools.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
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
__global__ static void CUDAKernelAddOneToVector(int* data)
{
  const int x  = blockIdx.x * blockDim.x + threadIdx.x;
  const int y  = blockIdx.y * blockDim.y + threadIdx.y;
  const int mx = gridDim.x * blockDim.x;

  data[y * mx + x] = data[y * mx + x] + 1;
}

void Tools::AdjustContrastBrightness(cv::Mat& img, double alpha, int beta)
{
  for (int y = 0; y < img.rows; y++)
  {
    for (int x = 0; x < img.cols; x++)
    {
      for (int c = 0; c < 3; c++)
      {
        img.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(
            alpha * (img.at<cv::Vec3b>(y, x)[c]) + beta);
      }
    }
  }
}

}  // namespace cuda
}  // namespace jcy
