// https://odinthenerd.blogspot.com/2017/03/recursive-alias-pattern-why-kvasirmpl.html


template<unsigned>
struct fold_impl;

template<>
struct fold_impl<0>
{
  template<template<typename...> class F, typename T>
  using type = T;
};

template<>
struct fold_impl<1>
{
  template<template<typename...> class F, typename T, typename U, typename...Ts>
  using type = typename fold_impl<(sizeof...(Ts) > 0)>::template type<F, F<T, U>, Ts...>;
};

template<template<typename...> class F, typename T, typename U, typename...Ts>
using fold = typename fold_impl<1>::template type<F, T, U, Ts...>;


template<int I>
struct int_ { static constexpr int value = I; };

template<typename T, typename U>
using add = int_<(T::value + U::value)>;


template<typename T> struct debug_t;


int main()
{
  using result = fold<add, int_<1>, int_<2>, int_<3>, int_<4>, int_<5> >;
  debug_t<result> d;
}
