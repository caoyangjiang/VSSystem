// Copyright 2017 Caoyang Jiang

#ifndef MODULES_DIP_INCLUDE_JCY_DIP_TOOLS_H_
#define MODULES_DIP_INCLUDE_JCY_DIP_TOOLS_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
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
/**
 * @brief      Class for general image processing
 * @ingroup    CodecUtility
 */
class Tools
{
 public:
  /**
   * @brief         Adjust iamge contrast and brightness
   *
   * @param[in,out] img    The image
   * @param[in]     alpha  The alpha. A floating number adjusts global contrast
   * @param[in]     beta   The beta. A integer adjusts global brightness
   */
  static void AdjustContrastBrightness(cv::Mat& img,
                                       double alpha = 1.5,
                                       int beta     = 20);

  /**
   * @brief      Merge multiple images into one.
   *
   * @param      imgs    Input images
   * @param[in]  column  The number images per column. (arrangment)
   * @param      merged  The merged
   */
  static void Merge(std::vector<cv::Mat>& imgs, int column, cv::Mat& merged);

  /**
   * @brief      Shows the image.
   *
   * @param      img         The image
   * @param[in]  windowname  The windowname
   */
  static void ShowImage(cv::Mat& img, const std::string& windowname);

  /**
   * @brief      Cpu-based denoising. Not recommended for real-time application
   *
   * @param[in]  in          Input image
   * @param[in]  noisesigma  The noise sigma
   * @param[out] out         Output image
   */
  static void Denoising(cv::Mat& in, int noisesigma, cv::Mat& out);
};

}  // namespace jcy

#endif  // MODULES_DIP_INCLUDE_JCY_DIP_TOOLS_H_
