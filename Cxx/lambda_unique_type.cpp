#include <type_traits>

template<typename E0, typename E1, auto UQ> struct Binary {};

template<typename E0, typename E1, auto UQ = []{}> 
auto add(E0 &&e0, E1 &&e1) -> Binary<E0, E1, UQ>
{
  return {};
}

int main()
{
  auto a = add(1, 2);
  auto b = add(1, 2);
  static_assert(!std::is_same_v<decltype(a), decltype(b)>);
}
