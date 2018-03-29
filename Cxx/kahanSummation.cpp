#include <iostream>
#include <array>
#include <vector>
#include <stdlib.h>


template<class T>
T basicSummation(T const * const data,
                 std::size_t const &size)
{
  T result(0);

  for(std::size_t it = 0; it != size; ++it)
    result += data[it];

  return result;
}


template<class T>
long double kahanSummation(T const * const data,
                           std::size_t const &size)
{
  long double result(0);
  long double c(0);

  for(std::size_t it = 0; it != size; ++it)
    {
      long double const y(data[it] - c);
      long double const t(result + y);
      c = (t - result) - y;
      result = t;
    }

  return result;
}



int main()
{
  std::vector<float> data;
  data.resize(1000000);
  for (auto &e: data) { e = rand() / (rand() * 1.0 + 1.0); }

  float bsum = basicSummation(data.data(), data.size());
  float ksum = kahanSummation(data.data(), data.size());
  std::cout << bsum << std::endl;
  std::cout << ksum << std::endl;
  std::cout << ksum - bsum << std::endl;
}
