#include <iostream>
#include <array>
#include <cassert>


void eval(double const &a,
          double const &b,
          double &z)
{
  z = a * b;
}


template<int size>
void eval_md(double const &a,
             std::array<double, size> const &a_md,
             double const &b,
             std::array<double, size> const &b_md,
             std::array<double, size> &z_md)
{
  for (int it = 0; it != size; ++it)
    z_md[it] = a_md[it] * b + a * b_md[it];
}


int main()
{
  double const a{3};
  double const b{4};
  double z{};
  eval(a, b, z);
  assert(z == 12);

  int constexpr size{2};
  std::array<double, size> const a_md{1, 0};
  std::array<double, size> const b_md{0, 1};
  std::array<double, size> z_md{};
  eval_md<size>(a, a_md, b, b_md, z_md);
  assert(z_md[0] == 4);
  assert(z_md[1] == 3);
}
