#include <type_traits>

template<typename T, T t> struct X {};

template<typename L , typename R, auto Unique = []{}> 
auto f(L const &l , R const &r) -> X<decltype(Unique), Unique>
{
  return {};
}

int main()
{
  auto a = f(1, 2);
  auto b = f(1, 2);
  static_assert(!std::is_same<decltype(a), decltype(b)>::value, "");
}
