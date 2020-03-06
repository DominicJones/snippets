#include <tuple>
#include <iostream>


namespace TupleHelper
{
  namespace detail
  {
    template<class T>
    struct init_tuple
    {
      template<class Ti>
      static inline constexpr auto
      eval(Ti v) noexcept
      {
        return T{T(v)};
      }
    };

    template<template<class...> class TP, class... T>
    struct init_tuple<TP<T...> >
    {
      template<class Ti>
      static inline constexpr auto
      eval(Ti v) noexcept
      {
        return TP<T...>{T{T(v)}...};
      }
    };
  }

  template<class TP, class Ti>
  inline constexpr auto
  init_tuple(Ti v) noexcept
  {
    return detail::init_tuple<TP>::eval(v);
  }
}


int main()
{
  using Tx = std::tuple<int, float>;
  auto const x{TupleHelper::init_tuple<Tx>(1)};
  std::cout << std::get<0>(x) << std::endl;
  std::cout << std::get<1>(x) << std::endl;

  using Ty = float;
  auto const y{TupleHelper::init_tuple<Ty>(1)};
  std::cout << y << std::endl;
}
