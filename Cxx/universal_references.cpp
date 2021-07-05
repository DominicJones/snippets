#include <iostream>
#include <type_traits>

struct Plus;

template<typename OP, typename E0> struct Unary
{
  Unary(E0 &&e0) : e0(e0) {}
  E0 e0;
};

template<typename E0> auto plus(E0 &&e0)
{
  return Unary<Plus, E0>(std::forward<E0>(e0));
}

int main()
{
  auto const value = 1;
  auto p = plus(int(value));
  static_assert(std::is_same<decltype(p.e0), int >::value, "");
  std::cout << p.e0 << std::endl;
}
