// compile with: g++ -std=c++11 -O3

/*
  Typical output for numeric_type = int32_t:

  numeric_type size [bytes] = 4

  ordered array access:
  segregated, 3 args:
  duration [ms] = 53
  duration [ms] = 54
  duration [ms] = 55
  interleaved, 3 packed args:
  duration [ms] = 42
  duration [ms] = 43
  duration [ms] = 42
  segregated, 6 args:
  duration [ms] = 74
  duration [ms] = 73
  duration [ms] = 73
  interleaved, 6 packed args:
  duration [ms] = 55
  duration [ms] = 55
  duration [ms] = 55
  semi-interleaved, 3 packed args, x2:
  duration [ms] = 61
  duration [ms] = 61
  duration [ms] = 61

  shuffled array access:
  segregated, 3 args:
  duration [ms] = 513
  duration [ms] = 514
  duration [ms] = 513
  interleaved, 3 packed args:
  duration [ms] = 395
  duration [ms] = 395
  duration [ms] = 395
  segregated, 6 args:
  duration [ms] = 753
  duration [ms] = 752
  duration [ms] = 753
  interleaved, 6 packed args:
  duration [ms] = 400
  duration [ms] = 400
  duration [ms] = 401
  semi-interleaved, 3 packed args, x2:
  duration [ms] = 444
  duration [ms] = 444
  duration [ms] = 445
*/


#include <iostream>
#include <array>
#include <vector>
#include <numeric>
#include <chrono>
#include <cstdint>
#include <algorithm>


using numeric_type = int32_t;


void test_segregated(std::vector<size_t> const &offset,
                     std::vector<numeric_type> const &arg0,
                     std::vector<numeric_type> const &arg1,
                     std::vector<numeric_type> const &arg2,
                     std::vector<numeric_type> &result)
{
  for (size_t it = 0; it != result.size(); ++it)
  {
    size_t const id(offset[it]);
    std::array<numeric_type, 3> const argsL({arg0[id], arg1[id], arg2[id]});
    numeric_type const resultL(std::accumulate(argsL.begin(), argsL.end(), numeric_type(0)));
    result[id] = resultL;
  }
}


void test_segregated(std::vector<size_t> const &offset,
                     std::vector<numeric_type> const &arg0,
                     std::vector<numeric_type> const &arg1,
                     std::vector<numeric_type> const &arg2,
                     std::vector<numeric_type> const &arg3,
                     std::vector<numeric_type> const &arg4,
                     std::vector<numeric_type> const &arg5,
                     std::vector<numeric_type> &result)
{
  for (size_t it = 0; it != result.size(); ++it)
  {
    size_t const id(offset[it]);
    std::array<numeric_type, 6> const argsL({arg0[id], arg1[id], arg2[id], arg3[id], arg4[id], arg5[id]});
    numeric_type const resultL(std::accumulate(argsL.begin(), argsL.end(), numeric_type(0)));
    result[id] = resultL;
  }
}


void test_interleaved(std::vector<size_t> const &offset,
                      std::vector<std::array<numeric_type, 3> > const &args,
                      std::vector<numeric_type> &result)
{
  for (size_t it = 0; it != result.size(); ++it)
  {
    size_t const id(offset[it]);
    std::array<numeric_type, 3> const argsL(args[id]);
    numeric_type const resultL(std::accumulate(argsL.begin(), argsL.end(), numeric_type(0)));
    result[id] = resultL;
  }
}


void test_interleaved(std::vector<size_t> const &offset,
                      std::vector<std::array<numeric_type, 6> > const &args,
                      std::vector<numeric_type> &result)
{
  for (size_t it = 0; it != result.size(); ++it)
  {
    size_t const id(offset[it]);
    std::array<numeric_type, 6> const argsL(args[id]);
    numeric_type const resultL(std::accumulate(argsL.begin(), argsL.end(), numeric_type(0)));
    result[id] = resultL;
  }
}


void test_semi_interleaved(std::vector<size_t> const &offset,
                           std::vector<std::array<numeric_type, 3> > const &args0,
                           std::vector<std::array<numeric_type, 3> > const &args1,
                           std::vector<numeric_type> &result)
{
  for (size_t it = 0; it != result.size(); ++it)
  {
    size_t const id(offset[it]);
    std::array<numeric_type, 6> const argsL({args0[id][0], args0[id][1], args0[id][2], args1[id][0], args1[id][1], args1[id][2]});
    numeric_type const resultL(std::accumulate(argsL.begin(), argsL.end(), numeric_type(0)));
    result[id] = resultL;
  }
}


int main()
{
  std::cout << "numeric_type size [bytes] = " << sizeof(numeric_type) << std::endl;

  size_t const size(16 * 1024 * 1024);
  std::vector<size_t> offset(size);

  for (size_t shuffle = 0; shuffle != 2; ++shuffle)
  {
    if (shuffle == false)
    {
      std::cout << std::endl;
      std::cout << "ordered array access:" << std::endl;
      for (size_t it = 0; it != size; ++it)
        offset.push_back(it);
    }

    if (shuffle == true)
    {
      std::cout << std::endl;
      std::cout << "shuffled array access:" << std::endl;
      std::random_shuffle(offset.begin(), offset.end());
    }

    {
      std::vector<numeric_type> arg0(size, 0);
      std::vector<numeric_type> arg1(size, 0);
      std::vector<numeric_type> arg2(size, 0);

      std::vector<std::array<numeric_type, 3> > args;
      args.assign(size, std::array<numeric_type, 3>{0, 0, 0});

      std::vector<numeric_type> result(size, 0);

      std::cout << "segregated, 3 args:" << std::endl;
      for (size_t it = 0; it != 3; ++it)
      {
        auto const start(std::chrono::high_resolution_clock::now());
        test_segregated(offset, arg0, arg1, arg2, result);
        auto const stop(std::chrono::high_resolution_clock::now());
        auto const duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
        std::cout << "duration [ms] = " << duration.count() << std::endl;
      }

      std::cout << "interleaved, 3 packed args:" << std::endl;
      for (size_t it = 0; it != 3; ++it)
      {
        auto const start(std::chrono::high_resolution_clock::now());
        test_interleaved(offset, args, result);
        auto const stop(std::chrono::high_resolution_clock::now());
        auto const duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
        std::cout << "duration [ms] = " << duration.count() << std::endl;
      }
    }

    {
      std::vector<numeric_type> arg0(size, 0);
      std::vector<numeric_type> arg1(size, 0);
      std::vector<numeric_type> arg2(size, 0);
      std::vector<numeric_type> arg3(size, 0);
      std::vector<numeric_type> arg4(size, 0);
      std::vector<numeric_type> arg5(size, 0);

      std::vector<std::array<numeric_type, 3> > args0;
      args0.assign(size, std::array<numeric_type, 3>{0, 0, 0});

      std::vector<std::array<numeric_type, 3> > args1;
      args1.assign(size, std::array<numeric_type, 3>{0, 0, 0});

      std::vector<std::array<numeric_type, 6> > args;
      args.assign(size, std::array<numeric_type, 6>{0, 0, 0, 0, 0, 0});

      std::vector<numeric_type> result(size, 0);

      std::cout << "segregated, 6 args:" << std::endl;
      for (size_t it = 0; it != 3; ++it)
      {
        auto const start(std::chrono::high_resolution_clock::now());
        test_segregated(offset, arg0, arg1, arg2, arg3, arg4, arg5, result);
        auto const stop(std::chrono::high_resolution_clock::now());
        auto const duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
        std::cout << "duration [ms] = " << duration.count() << std::endl;
      }

      std::cout << "interleaved, 6 packed args:" << std::endl;
      for (size_t it = 0; it != 3; ++it)
      {
        auto const start(std::chrono::high_resolution_clock::now());
        test_interleaved(offset, args, result);
        auto const stop(std::chrono::high_resolution_clock::now());
        auto const duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
        std::cout << "duration [ms] = " << duration.count() << std::endl;
      }

      std::cout << "semi-interleaved, 3 packed args, x2:" << std::endl;
      for (size_t it = 0; it != 3; ++it)
      {
        auto const start(std::chrono::high_resolution_clock::now());
        test_semi_interleaved(offset, args0, args1, result);
        auto const stop(std::chrono::high_resolution_clock::now());
        auto const duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
        std::cout << "duration [ms] = " << duration.count() << std::endl;
      }
    }
  }
}
