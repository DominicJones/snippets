#include <type_traits>

template<typename T, auto = [](T){}> struct Unique {};

/*
  This needs to be a language feature,
  which can be invoked in a constexpr context and
  which is used like:
    varid(x) -> std::size_t
*/
template<typename T>
auto constexpr varid()
{
  return Unique<T>{};
}

template<typename OP, typename E0, typename E1, auto I0, auto I1>
struct Binary {};

struct Add;

/*
  This should be written as:
    template<typename E0, typename E1> 
    auto add(E0 const &e0, E1 const &e1)
     -> Binary<Add, E0, E1, varid(e0), varid(e1)>
*/
template<typename E0, typename E1> 
auto add(E0 const &e0, E1 const &e1)
 -> Binary<Add, E0, E1, varid<E0>(), varid<E1>()>
{
  return {};
}

int main()
{
  double a;
  double b;

  auto x = add(a, b);
  auto y = add(b, a);

/*
  This tests will only pass if varid is implemented correctly
    static_assert(!std::is_same_v<decltype(x), decltype(y)>);
*/
}
