// encode the value inside the type without the value being part of the type.
// https://mpark.github.io/programming/2017/05/26/constexpr-function-parameters/

#include <tuple>
#include <type_traits>

template<typename X>
auto constexpr f(X)
{
  static_assert(std::get<0>(X::value()) == 101, "");
  static_assert(std::get<1>(X::value()) == 202, "");
}

template<typename X>
auto constexpr g(X x)
{
  static_assert(std::get<0>(x()) == 101, "");
  static_assert(std::get<1>(x()) == 202, "");
}

int main()
{
  struct S { static auto constexpr value() { return std::make_tuple(101, 202); } };
  f(S{});

  g([]{ return std::make_tuple(101, 202); });
}
