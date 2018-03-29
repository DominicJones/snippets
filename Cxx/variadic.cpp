template<typename... list>
struct count;

template<>
struct count<>
{
  static const int value = 0;
};

template<typename head, typename... tail>
struct count<head, tail...>
{
  static const int value = 1 + count<tail...>::value;
};


template<int... list>
struct sum;

template<>
struct sum<>
{
  static const int value = 0;
};

template<int head, int... tail>
struct sum<head, tail...>
{
  static const int value = head + sum<tail...>::value;
};


#include <cassert>
int main()
{
  assert((count<char>::value) == 1);
  assert((count<float, double>::value) == 2);
  assert((count<short, int, long>::value) == 3);

  assert((sum<>::value) == 0);
  assert((sum<2>::value) == 2);
  assert((sum<2, 4>::value) == 6);
  assert((sum<2, 4, 6>::value) == 12);
}
