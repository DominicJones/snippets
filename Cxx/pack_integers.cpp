#include <iostream>
#include <functional>
#include <cmath>

int main()
{
  int base = 8*8;

  int i0 = 35;
  int i1 = 16;

  int i01 = i0 + i1 * base;

  int j1 = i01%base;
  int j0 = (i01 - j1)/base;

  std::cout << j0 << std::endl;
  std::cout << j1 << std::endl;
}
