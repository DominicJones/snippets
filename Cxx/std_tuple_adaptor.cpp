#include <tuple>


namespace mpl
{
  template<class...> struct tuple {};
}


namespace mpl
{
  namespace lazy
  {
    template <class A, template<class...> class B> struct wrap;

    template<template<class...> class A, class... T, template<class...> class B>
    struct wrap<A<T...>, B>
    {
      using type = B<T...>;
    };
  }

  template<class A, template<class...> class B>
  using wrap = typename lazy::wrap<A, B>::type;
}


namespace mpl
{
  template <typename... T>
  using tuple_wrapper = typename std::tuple<T...>;

  template <typename L>
  using as_tuple = wrap<L, tuple_wrapper>;
}


template<class T> struct debug;

int main()
{
  using t = std::tuple<int, float>;
  using u = mpl::tuple<int, float>;

  using v = mpl::tuple_wrapper<int, float>;

  using w = mpl::as_tuple<t>;
  using x = mpl::as_tuple<u>;

  debug<x>{};
}
