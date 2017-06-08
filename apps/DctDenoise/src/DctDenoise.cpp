// Copyright 2017 Caoyang Jiang

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include "Jcy/DIP/CudaTools.h"
#include "Jcy/DIP/Tools.h"
#include "opencv2/opencv.hpp"

void ThreadWork(int id, int total)
{
  for (int i = 1; i < 900; i++)
  {
    if ((i % total) == id)
    {
      cv::Mat in, out;

      std::cout << "Thread " << id << " process " << i << std::endl;
      std::string inname  = "./Save/Image" + std::to_string(i) + ".jpg";
      std::string outname = "./Save/DeImage" + std::to_string(i) + ".jpg";
      in                  = cv::imread(inname, 1);
      jcy::Tools::Denoising(in, 6, out);
      cv::imwrite(outname, out);
    }
  }
}

int main(int, char**)
{
  // std::thread::hardware_concurrency();
  // std::vector<std::thread> threads;
  // int total = std::thread::hardware_concurrency();

  // for (int i = 0; i < total; i++)
  // {
  //   threads.push_back(std::thread(ThreadWork, i, total));
  // }

  // for (int i = 0; i < total; i++)
  // {
  //   threads[i].join();
  // }

  // std::cout << "Denoising complete" << std::endl;

  for (size_t i = 1; i < 900; i++)
  {
    cv::Mat in, dein;

    std::string inname   = "./Save/Image" + std::to_string(i) + ".jpg";
    std::string deinname = "./Save/DeImage" + std::to_string(i) + ".jpg";
    in                   = cv::imread(inname, 1);
    dein                 = cv::imread(deinname, 1);
    jcy::Tools::ShowImage(in, "Original");
    jcy::Tools::ShowImage(dein, "Denoised");
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
  }

  return 0;
}
