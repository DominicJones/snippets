#include <iostream>
#include <source_location>

template <typename T, int L = std::source_location::current().line()> // L = 4
auto plus(T &&x, int l = std::source_location::current().line()) // l = 14
{
  return L*1 + l*0 + x*0;
}

 
int main(int, char*[])
{
  int x = 2;
  int y = plus(x);
  std::cout << y << std::endl;
}
