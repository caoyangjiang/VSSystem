// Copyright 2017 Caoyang Jiang

#include "Jcy/DIP/Tools.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <stdint.h>
#include <x264.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
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

void Tools::Merge(std::vector<cv::Mat>& imgs, int column, cv::Mat& merged)
{
  for (size_t i = 0; i < imgs.size() - 1; i++)
  {
    if ((imgs[i].size().width != imgs[i + 1].size().width) ||
        (imgs[i].size().height != imgs[i + 1].size().height))
    {
      throw std::runtime_error(
          "Tools::Merge failed. Input images has different size.");
    }
  }

  int rows = imgs.size() / column;
  merged   = cv::Mat::zeros(
      cv::Size(imgs[0].size().width * column, imgs[0].size().height * rows),
      imgs[0].type());

  int sequenceid = 0;
  for (int r = 0; r < rows; r++)
  {
    for (int c = 0; c < column; c++)
    {
      cv::Mat subimg(merged,
                     cv::Rect(c * imgs[sequenceid].size().width,
                              r * imgs[sequenceid].size().height,
                              imgs[sequenceid].size().width,
                              imgs[sequenceid].size().height));

      imgs[sequenceid].copyTo(subimg);
      sequenceid++;
    }
  }
}
}  // namespace jcy
