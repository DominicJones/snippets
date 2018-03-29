/* evaluate a function prior to passing an argument */

#include <iostream>

void bound(int &val)
{
  if (val == 0)
    val = 1;
}


int eval(int &val)
{
  return val / 2;
}


int main()
{
  int val = 0;
  eval((bound(val), val));
  std::cout << val << std::endl;
}
