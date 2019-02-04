#include "stdio.h"
  
int main()
{
  unsigned short c = 0;
  unsigned short c_plus_1 = 1;

  while (c_plus_1 == c + 1)
  {
    c = c + 1;
    c_plus_1 = c_plus_1 + 1;
  }
  
  printf("%d", c);
}
