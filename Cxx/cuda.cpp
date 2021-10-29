// -*- C++ -*-

// a helpful document:
//   https://ntrs.nasa.gov/api/citations/20180003378/downloads/20180003378.pdf

// bashrc config:
//   export CUDA_HOME=[[SOMEWHERE]]/cuda/11.0.2/linux-x86_64
//   export CUDACC_HOME=$CUDA_HOME/cuda_nvcc
//   export CUDART_HOME=$CUDA_HOME/cuda_cudart
//   export LD_LIBRARY_PATH=$CUDART_HOME/lib64:$LD_LIBRARY_PATH
//   export PATH=$CUDACC_HOME/bin:$PATH

// compilation:
//   GPU:
//     nvcc -x cu -std=c++14 -I$CUDART_PATH/include -L$CUDART_PATH/lib64 <file>.cpp
//   CPU:
//     g++ -std=c++14 <file>.cpp


#include <iostream>
#include <chrono>
#include <vector>


#if defined(__CUDACC__)
#include "cuda_runtime.h"
#include "thrust/host_vector.h"
#include "thrust/device_vector.h"
#endif


#if defined(__CUDACC__)
#define CUDA_HOST __host__
#define CUDA_DEVICE __device__
#define CUDA_GLOBAL __global__
#else
#define CUDA_HOST
#define CUDA_DEVICE
#define CUDA_GLOBAL
#endif


#if defined(__CUDACC__)
template<typename T> using HostVector = thrust::host_vector<T>;
template<typename T> using DeviceVector = thrust::device_vector<T>;
#else
template<typename T> using HostVector = std::vector<T>;
template<typename T> using DeviceVector = std::vector<T>;
#endif


#if defined(__CUDA_ARCH__)
#define ForEach(it, n_it)                       \
  int it = blockIdx.x*blockDim.x + threadIdx.x; \
  if (it < n_it)
#else
#define ForEach(it, n_it)                       \
  for (int it = 0; it != n_it; ++it)
#endif


CUDA_HOST CUDA_DEVICE
void saxpy_impl(int n, float a, float * x, float * y)
{
  ForEach(i, n)
  {
    y[i] = a*x[i] + y[i];
  }
}


CUDA_GLOBAL
void saxpy(int n, float a, float * x, float * y)
{
  saxpy_impl(n, a, x, y);
}


int main()
{
  using Clock = std::chrono::high_resolution_clock;

  int N = 100000000;

#if defined(__CUDACC__)
  int n_blocks = 0;
  int n_threads_per_block = 0;
  cudaOccupancyMaxPotentialBlockSize(&n_blocks, &n_threads_per_block, saxpy, 0, N);
  n_blocks = (N + n_threads_per_block - 1) / n_threads_per_block;
  std::cout << "n_blocks = " << n_blocks << std::endl;
  std::cout << "n_threads_per_block = " << n_threads_per_block << std::endl;
#endif

  HostVector<float> hx, hy;
  hx.resize(N);
  hy.resize(N);

  for (int i = 0; i != N; ++i) {
    hx[i] = 1;
    hy[i] = 2;
  }

  auto timer_start = Clock::now();

#if defined(__CUDACC__)
  DeviceVector<float> dx, dy;
  dx = hx;
  dy = hy;
  auto dxp = thrust::raw_pointer_cast(&dx[0]);
  auto dyp = thrust::raw_pointer_cast(&dy[0]);
  saxpy<<<n_blocks, n_threads_per_block>>>(N, 2, dxp, dyp);
  hy = dy;
#else
  auto dxp = (&hx[0]);
  auto dyp = (&hy[0]);
  saxpy(N, 2, dxp, dyp);
#endif

  auto timer_end = Clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timer_end - timer_start).count();

  float maxError = 0;
  for (int i = 0; i != N; ++i) {
    maxError = std::max(maxError, std::abs(hy[i] - 4));
  }

  std::cout << "max error: " << maxError << std::endl;
  std::cout << "timer: "  << duration << " ms" << std::endl;
}
