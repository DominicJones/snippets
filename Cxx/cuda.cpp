#include "cuda_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

#define ITER 65535


void vector_add_cpu(int *a, int *b, int *c, int n)
{
    for (int i = 0; i < n; ++i) c[i] = a[i] + b[i];
}


__global__ void vector_add_gpu(int *gpu_a, int *gpu_b, int *gpu_c, int n)
{
    // No for loop needed because the CUDA runtime
    // will thread this ITER times
    int i = threadIdx.x;
    gpu_c[i] = gpu_a[i] + gpu_b[i];
}


int main()
{
    int *a, *b, *c;
    int *gpu_a, *gpu_b, *gpu_c;

    a = (int *)malloc(ITER * sizeof(int));
    b = (int *)malloc(ITER * sizeof(int));
    c = (int *)malloc(ITER * sizeof(int));

    cudaMallocManaged(&gpu_a, ITER * sizeof(int));
    cudaMallocManaged(&gpu_b, ITER * sizeof(int));
    cudaMallocManaged(&gpu_c, ITER * sizeof(int));

    for (int i = 0; i < ITER; ++i)
    {
        a[i] = i;
        b[i] = i;
        c[i] = i;
    }

    auto cpu_start = Clock::now();
    vector_add_cpu(a, b, c, ITER);
    auto cpu_end = Clock::now();
    std::cout << "vector_add_cpu: "
      << std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_end - cpu_start).count()
      << " nanoseconds.\n";

    // The triple angle brakets is a CUDA runtime extension that allows
    // parameters of a CUDA kernel call to be passed.
    // In this example, we are passing one thread block with ITER threads.
    auto gpu_start = Clock::now();
    vector_add_gpu <<<1, ITER>>> (gpu_a, gpu_b, gpu_c, ITER);
    cudaDeviceSynchronize();
    auto gpu_end = Clock::now();
    std::cout << "vector_add_gpu: "
      << std::chrono::duration_cast<std::chrono::nanoseconds>(gpu_end - gpu_start).count()
      << " nanoseconds.\n";

    cudaFree(a);
    cudaFree(b);
    cudaFree(c);

    free(a);
    free(b);
    free(c);
}


/* Makefile (dep: sudo apt-get install cuda -y)
INC=-I/usr/local/cuda/include
NVCC=/usr/local/cuda/bin/nvcc
NVCC_OPT=-std=c++11

all:
    $(NVCC) $(NVCC_OPT) $(INC) -c <file>.cpp
*/
