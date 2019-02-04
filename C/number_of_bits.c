#include "stdio.h"

int main()
{
  unsigned long c = 0;
  unsigned long c_max = 65535;

  unsigned long order  = 1;
  unsigned long n_bits = 0;

  while (c < c_max)
  {
    c = c + order;
    order = 2 * order;
    n_bits = n_bits + 1;
  }

  printf("%lu", n_bits);
}
