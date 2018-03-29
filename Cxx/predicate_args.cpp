#include <type_traits>
#include <typeinfo>
#include <iostream>


struct _1;
struct _2;

template<class...> struct list;

template<template<class...> class, class...> struct bind;

template<bool B> using bool_t = std::integral_constant<bool, B>;

template<class L, class P> struct partition;

template<template<class...> class L, class T, class... Ts, template<class...> class F>
struct partition<L<T, Ts...>, bind<F, _1, _2> >
{
  using type = L<F<T, Ts>...>;
};


template<typename T0, typename T1>
using incr = bool_t<(sizeof(T1) > sizeof(T0))>;

int main()
{
  using L = list<int, bool, char, double>;
  using R = partition<L, bind<incr, _1, _2> >::type;
  using E = list<bool_t<false>, bool_t<false>, bool_t<true> >;
  static_assert(std::is_same<R, E>::value, "");
}
