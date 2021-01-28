// -*- C++ -*-
#pragma once


#ifdef USE_SIMPLETUPLE

namespace simpletuple
{
  // tuple

  namespace detail
  {
    template<std::size_t, typename...> struct tuple_impl;

    template<std::size_t I> struct tuple_impl<I> {};

    template<std::size_t I, typename T0, typename... Ts>
    struct tuple_impl<I, T0, Ts...>
      : tuple_impl<I + 1, Ts...>
    {
      using base_t = tuple_impl<I + 1, Ts...>;
      inline constexpr tuple_impl() noexcept : base_t() {}
      inline constexpr tuple_impl(T0 && value, Ts &&... values) noexcept
        : base_t(std::forward<Ts>(values)...)
        , value(std::forward<T0>(value))
      {}

      T0 value;
    };
  }

  template<typename... Ts>
  struct tuple
    : detail::tuple_impl<0, Ts...>
  {
    using base_t = detail::tuple_impl<0, Ts...>;
    inline constexpr tuple() noexcept : base_t() {}
    inline constexpr tuple(Ts &&... values) noexcept
      : base_t(std::forward<Ts>(values)...)
    {}
  };

  template<> struct tuple<> { tuple(){} };


  // tuple_element

  template<std::size_t, typename> struct tuple_element;

  template<std::size_t I, typename T0, typename... Ts>
  struct tuple_element<I, tuple<T0, Ts...> >
    : tuple_element<I - 1, tuple<Ts...> >
  {};

  template<typename T0, typename... Ts>
  struct tuple_element<0, tuple<T0, Ts...> >
  {
    using type = T0;
  };

  template<std::size_t I, class T>
  using tuple_element_t = typename tuple_element<I, T>::type;


  // get<I> (non-const)

  namespace detail
  {
    template<std::size_t I, typename T0, typename... Ts>
    inline constexpr T0 &
    get_impl(tuple_impl<I, T0, Ts...> &t) noexcept
    {
      return t.value;
    }
  }

  template<std::size_t I, typename... Ts>
  inline constexpr tuple_element_t<I, tuple<Ts...> > &
  get(tuple<Ts...> &t) noexcept
  {
    return detail::get_impl<I>(t);
  }


  // get<I> (const)

  namespace detail
  {
    template<std::size_t I, typename T0, typename... Ts>
    inline constexpr T0 const &
    get_impl(tuple_impl<I, T0, Ts...> const &t) noexcept
    {
      return t.value;
    }
  }

  template<std::size_t I, typename... Ts>
  inline constexpr tuple_element_t<I, tuple<Ts...> > const &
  get(tuple<Ts...> const &t) noexcept
  {
    return detail::get_impl<I>(t);
  }
}

#else

#include <tuple>

namespace simpletuple = std;

#endif
