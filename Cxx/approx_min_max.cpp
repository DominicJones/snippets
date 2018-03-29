#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>


int main()
{
  std::array<double, 5> const f{10, 20, 30, 40, 50};
  double const f_max(*std::max_element(f.begin(), f.end()));
  int const f_size(f.size());

  for (int expn = 1; expn != 8; ++expn)
  {
    double r(0);
    for (auto const &v: f) { r += std::pow((v / f_max), expn); }
    double const n(f_max * std::pow(r / f_size, 1.0 / expn));
    std::cout << "max: expn: " << expn << ", value: " << n << std::endl;
  }

  for (int expn = 1; expn != 8; ++expn)
  {
    double r(0);
    for (auto const &v: f) { r += std::pow(1.0 / (v / f_max), expn); }
    double const n(f_max * std::pow((1.0 / r) / f_size, 1.0 / expn));
    std::cout << "min: expn: " << expn << ", value: " << n << std::endl;
  }
}
