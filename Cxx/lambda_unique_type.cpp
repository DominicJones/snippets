#include <type_traits>

template<typename T, auto UQ = []{}>
struct Terminal {};

template<typename OP, typename E0, typename E1>
struct Binary {};

struct Add;
template<typename E0, typename E1> 
auto add(E0 &&e0, E1 &&e1) -> Binary<Add, E0, E1>
{
  return {};
}

int main()
{
  Terminal<double> a;
  Terminal<double> b;
  auto x = add(a, b);
  auto y = add(a, b);
  auto z = add(b, a);
  static_assert(!std::is_same_v<decltype(a), decltype(b)>);
  static_assert(std::is_same_v<decltype(x), decltype(y)>);
  static_assert(!std::is_same_v<decltype(y), decltype(z)>);
}
