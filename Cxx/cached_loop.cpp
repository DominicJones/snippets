// compile with: g++ -std=c++11 -O3

/*
  Typical output for numeric_type = int32_t:

  numeric_type size [bytes] = 4
  uncached loop, 6 args:
  duration [ms] = 169
  duration [ms] = 169
  duration [ms] = 169
  cached loop, 6 args:
  duration [ms] = 177
  duration [ms] = 176
  duration [ms] = 176
*/


#include <iostream>
#include <array>
#include <vector>
#include <numeric>
#include <chrono>
#include <cstdint>
#include <cmath>


using numeric_type = int64_t;


template<class Iterator>
numeric_type expensive_function(Iterator begin, Iterator end)
{
  numeric_type result(0);
  double const expn(2.345);
  double const expnR(1 / double(expn));

  for (; begin != end; ++begin)
    result += std::pow(*begin, expn);

  return std::pow(result, expnR);
}


void test_uncached_loop(std::vector<numeric_type> const &arg0,
                        std::vector<numeric_type> const &arg1,
                        std::vector<numeric_type> const &arg2,
                        std::vector<numeric_type> const &arg3,
                        std::vector<numeric_type> const &arg4,
                        std::vector<numeric_type> const &arg5,
                        std::vector<numeric_type> &result)
{
  for (size_t it = 0; it != result.size() - 1; ++it)
  {
    std::array<numeric_type, 6> const cache({arg0[it], arg1[it], arg2[it], arg3[it], arg4[it], arg5[it]});
    result[it] = expensive_function(cache.begin(), cache.end());
  }
}


void test_cached_loop(std::vector<numeric_type> const &arg0,
                      std::vector<numeric_type> const &arg1,
                      std::vector<numeric_type> const &arg2,
                      std::vector<numeric_type> const &arg3,
                      std::vector<numeric_type> const &arg4,
                      std::vector<numeric_type> const &arg5,
                      std::vector<numeric_type> &result)
{
  std::array<bool, 2> isDirty = {true, true};
  std::array<std::array<numeric_type, 6>, 2> cache = {0};

  for (size_t it = 0; it != result.size() - 1; ++it)
  {
    uint8_t const r0((it & 1));
    uint8_t const r1(!(it & 1));

    size_t const it0(it + r0);
    size_t const it1(it + r1);

    if (isDirty[r0])
    {
      cache[r0] = {arg0[it0], arg1[it0], arg2[it0], arg3[it0], arg4[it0], arg5[it0]};
      isDirty[r0] = false;
    }

    if (isDirty[r1])
    {
      cache[r1] = {arg0[it1], arg1[it1], arg2[it1], arg3[it1], arg4[it1], arg5[it1]};
      isDirty[r1] = false;
    }

    result[it] = expensive_function(cache[0].begin(), cache[0].end());
    isDirty[0] = true;
  }
}


int main()
{
  std::cout << "numeric_type size [bytes] = " << sizeof(numeric_type) << std::endl;
  size_t const size(1 * 1024 * 1024);

  {
    std::vector<numeric_type> arg0;
    arg0.assign(size, 0);
    std::vector<numeric_type> arg1;
    arg1.assign(size, 0);
    std::vector<numeric_type> arg2;
    arg2.assign(size, 0);
    std::vector<numeric_type> arg3;
    arg3.assign(size, 0);
    std::vector<numeric_type> arg4;
    arg4.assign(size, 0);
    std::vector<numeric_type> arg5;
    arg5.assign(size, 0);

    std::vector<std::array<numeric_type, 6> > args;
    args.assign(size, std::array<numeric_type, 6>{0, 0, 0, 0, 0, 0});

    std::vector<numeric_type> result;
    result.assign(size, 0);

    std::cout << "uncached loop, 6 args:" << std::endl;
    for (size_t it = 0; it != 3; ++it)
    {
      auto const start(std::chrono::high_resolution_clock::now());

      test_uncached_loop(arg0, arg1, arg2, arg3, arg4, arg5, result);

      auto const stop(std::chrono::high_resolution_clock::now());
      auto const duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
      std::cout << "duration [ms] = " << duration.count() << std::endl;
    }

    std::cout << "cached loop, 6 args:" << std::endl;
    for (size_t it = 0; it != 3; ++it)
    {
      auto const start(std::chrono::high_resolution_clock::now());

      test_cached_loop(arg0, arg1, arg2, arg3, arg4, arg5, result);

      auto const stop(std::chrono::high_resolution_clock::now());
      auto const duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
      std::cout << "duration [ms] = " << duration.count() << std::endl;
    }
  }
}
