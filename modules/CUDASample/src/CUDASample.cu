// Copyright 2015 Jason Juang

#include "Hvr/CUDASample/CUDASample.cuh"

#include <vector>

#include "Hvr/CUDASample/CUDAConfig.h"

__global__ static void CUDAKernelAddOneToVector(int *data)
{
  const int x  = blockIdx.x * blockDim.x + threadIdx.x;
  const int y  = blockIdx.y * blockDim.y + threadIdx.y;
  const int mx = gridDim.x * blockDim.x;

  data[y * mx + x] = data[y * mx + x] + 1;
}

namespace hvr
{
CUDASample::CUDASample()
{
}

CUDASample::~CUDASample()
{
}

void CUDASample::AddOneToVector(std::vector<int> &in) const
{
  if (in.empty()) return;

  CUDAConfig cudaconfig;
  cudaconfig.w_a = static_cast<int>(in.size()) / cudaconfig.blk_h;
  cudaconfig.h_a = cudaconfig.blk_h;

  int *data;
  cudaMallocManaged(reinterpret_cast<void **>(&data),
                    in.size() * sizeof(int),
                    cudaMemAttachGlobal);

  for (int i = 0; i < static_cast<int>(in.size()); i++)
  {
    data[i] = in.at(i);
  }

  dim3 blks((cudaconfig.w_a >> cudaconfig.shift_w),
            (cudaconfig.h_a >> cudaconfig.shift_h));
  dim3 threads(cudaconfig.blk_w, cudaconfig.blk_h);

  CUDAKernelAddOneToVector<<<blks, threads>>>(data);

  cudaDeviceSynchronize();

  for (int i = 0; i < static_cast<int>(in.size()); i++)
  {
    in.at(i) = data[i];
  }

  cudaFree(data);
}

}  // namespace hvr
