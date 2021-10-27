// -*- C++ -*-

// nvcc -std=c++14 -I$CUDART_PATH/include -L$CUDART_PATH/lib64 <file>.cu

#include "cuda_runtime.h"
#include "thrust/host_vector.h"
#include "thrust/device_vector.h"

#include <iostream>
#include <chrono>
#include <vector>


__global__
void saxpy(int n, float a, float *x, float *y)
{
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < n)
  {
    y[i] = a*x[i] + y[i];
  }
}


int main()
{
  using Clock = std::chrono::high_resolution_clock;

  int N = 10000000;
  int n_threads_per_block = 256;
  int n_blocks = (N + n_threads_per_block - 1) / n_threads_per_block;

  thrust::host_vector<float> hx, hy;
  hx.resize(N);
  hy.resize(N);

  for (int i = 0; i < N; i++) {
    hx[i] = 1;
    hy[i] = 2;
  }

  thrust::device_vector<float> dx, dy;
  dx = hx;
  dy = hy;

  auto timer_start = Clock::now();

  auto dxp = thrust::raw_pointer_cast(&dx[0]);
  auto dyp = thrust::raw_pointer_cast(&dy[0]);
  saxpy<<<n_blocks, n_threads_per_block>>>(N, 2, dxp, dyp);

  auto timer_end = Clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(timer_end - timer_start).count();

  thrust::copy(dy.begin(), dy.end(), hy.begin());

  float maxError = 0;
  for (int i = 0; i < N; i++)
    maxError = max(maxError, abs(hy[i] - 4));

  std::cout << "max error: " << maxError << std::endl;
  std::cout << "timer: "  << duration << " ns" << std::endl;
}
