#include <utility>
#include <tuple>

auto pair()
{
  int a = 3;
  int b = 4;
  return std::make_pair(a, b);
}

auto average(int a, int b)
{
  return (a + b) / 2;
}

int main()
{
  auto r = std::apply(average, pair());
}
