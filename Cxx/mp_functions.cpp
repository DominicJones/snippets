// -*- C++ -*-

#ifndef _AlwaysInline_h_
#define _AlwaysInline_h_

#define AlwaysInlineGCC __attribute__((always_inline))

// #define AlwaysInline __attribute__((always_inline))

#define AlwaysInline
#define AlwaysInlineCtor AlwaysInlineGCC

#endif // _AlwaysInline_h_




// -*- C++ -*-

#ifndef _mp_tuple_h_
#define _mp_tuple_h_


// #define USE_STD_TUPLE

#ifdef USE_STD_TUPLE

#include <tuple>
template<typename... Ts> using mp_tuple = std::tuple<Ts...>;

template<std::size_t idx, typename... Ts> AlwaysInline inline constexpr decltype(auto) mp_get_v(mp_tuple<Ts...> const &t) { return std::get<idx>(t); }
template<std::size_t idx, typename... Ts> AlwaysInline inline constexpr decltype(auto) mp_get_r(mp_tuple<Ts...> const &t) { return(std::get<idx>(t)); }
template<std::size_t idx, typename... Ts> AlwaysInline inline constexpr decltype(auto) mp_get_r(mp_tuple<Ts...> &t) { return(std::get<idx>(t)); }

#else

#include "AlwaysInline.h"


// mp_tuple
template<std::size_t idx, typename... Ts>
struct mp_tuple_impl;

template<std::size_t idx>
struct mp_tuple_impl<idx>
{};

template<std::size_t idx, typename T1, typename... Ts>
struct mp_tuple_impl<idx, T1, Ts...>
  : mp_tuple_impl<idx + 1, Ts...>
{
  using base_t = mp_tuple_impl<idx + 1, Ts...>;

  AlwaysInline inline constexpr mp_tuple_impl() : base_t() {}

  AlwaysInline inline constexpr mp_tuple_impl(T1 value, Ts... values)
    : value(value)
    , base_t(values...)
  {}

  T1 value;
};

template<typename... Ts>
struct mp_tuple
  : mp_tuple_impl<0, Ts...>
{
  using base_t = mp_tuple_impl<0, Ts...>;
  AlwaysInline inline constexpr mp_tuple() : base_t() {}
  AlwaysInline inline constexpr mp_tuple(Ts... values) : base_t(values...) {}
};

template<>
struct mp_tuple<>
{
  AlwaysInline inline constexpr mp_tuple() {}
};


// mp_tuple_element
template<std::size_t idx, typename _Tp>
struct mp_tuple_element;

template<std::size_t idx, typename T1, typename... Ts>
struct mp_tuple_element<idx, mp_tuple<T1, Ts...> >
  : mp_tuple_element<idx - 1, mp_tuple<Ts...> >
{};

template<typename T1, typename... Ts>
struct mp_tuple_element<0, mp_tuple<T1, Ts...> >
{
  using type = T1;
};


// mp_get_v
template<std::size_t idx, typename T1, typename... Ts>
AlwaysInline inline constexpr T1 mp_get_v_impl(mp_tuple_impl<idx, T1, Ts...> const &t)
{
  return t.value;
}

template<std::size_t idx, typename... Ts>
AlwaysInline inline constexpr typename mp_tuple_element<idx, mp_tuple<Ts...> >::type
mp_get_v(mp_tuple<Ts...> const &t)
{
  return mp_get_v_impl<idx>(t);
}


// mp_get_r [non-const]
template<std::size_t idx, typename T1, typename... Ts>
AlwaysInline inline constexpr T1 &mp_get_r_impl(mp_tuple_impl<idx, T1, Ts...> &t)
{
  return t.value;
}

template<std::size_t idx, typename... Ts>
AlwaysInline inline constexpr typename mp_tuple_element<idx, mp_tuple<Ts...> >::type &
mp_get_r(mp_tuple<Ts...> &t)
{
  return mp_get_r_impl<idx>(t);
}


// mp_get_r [const]
template<std::size_t idx, typename T1, typename... Ts>
AlwaysInline inline constexpr T1 const &mp_get_r_impl(mp_tuple_impl<idx, T1, Ts...> const &t)
{
  return t.value;
}

template<std::size_t idx, typename... Ts>
AlwaysInline inline constexpr typename mp_tuple_element<idx, mp_tuple<Ts...> >::type const &
mp_get_r(mp_tuple<Ts...> const &t)
{
  return mp_get_r_impl<idx>(t);
}

#endif

#endif // _mp_tuple_h_




// -*- C++ -*-

/*
Based on:
  http://www.pdimov.com/cpp2/simple_cxx11_metaprogramming.html
  http://www.pdimov.com/cpp2/simple_cxx11_metaprogramming_2.html
  by Peter Dimov, 26.05.2015
*/


#ifndef _mp_functions_basic_h_
#define _mp_functions_basic_h_


// MIGRATE
#include "brigand.hpp"

#include "mp_tuple.h"

#include <tuple>
#include <type_traits>
#include <utility>


// mp_list
/*
template<class... T> struct mp_list {};
*/
template<class... T> using mp_list = brigand::list<T...>;


// mp_void
struct mp_void {};


// is_mp_void
template<class T> struct is_mp_void { static bool constexpr value = false; };
template<> struct is_mp_void<mp_void> { static bool constexpr value = true; };


// mp_identity
template<class T> struct mp_identity { using type = T; };
template<class T> struct mp_identity<mp_identity<T> > { using type = T; };

template<class T> using mp_identity_t = typename mp_identity<T>::type;


// mp_rename
template<class A, template<class...> class B> struct mp_rename_impl;

template<template<class...> class A, class... T, template<class...> class B>
struct mp_rename_impl<A<T...>, B>
{
    using type = B<T...>;
};

template<class A, template<class...> class B> using mp_rename = typename mp_rename_impl<A, B>::type;


// mp_length
template<class... T> using mp_length = std::integral_constant<std::size_t, sizeof...(T)>;


// mp_size
template<class L> using mp_size = mp_rename<L, mp_length>;


// mp_size_t
template<std::size_t N> using mp_size_t = std::integral_constant<std::size_t, N>;


// mp_apply
template<template<class...> class F, class L> using mp_apply = mp_rename<L, F>;


// mp_true
using mp_true = std::integral_constant<bool, true>;


// mp_false
using mp_false = std::integral_constant<bool, false>;


// mp_empty
template<class... L> struct mp_empty;

template<template<class...> class L>
struct mp_empty<L<> > : mp_true {};

template<template<class...> class L1, class... L>
struct mp_empty<L1<>, L...>
{
  static auto constexpr value = mp_empty<L...>::value;
};

template<template<class...> class L1, class T1, class... T, class... L>
struct mp_empty<L1<T1, T...>, L...> : mp_false {};


// mp_clear
template<class L> struct mp_clear_impl;

template<template<class...> class L, class... T>
struct mp_clear_impl<L<T...> >
{
  using type = L<>;
};

template<class L> using mp_clear = typename mp_clear_impl<L>::type;


// mp_front
template<class L> struct mp_front_impl;

template<template<class...> class L, class T1, class... T>
struct mp_front_impl<L<T1, T...> >
{
  using type = T1;
};

template<class L> using mp_front = typename mp_front_impl<L>::type;


// mp_pop_front
template<class L> struct mp_pop_front_impl;

template<template<class...> class L>
struct mp_pop_front_impl<L<> >
{
  using type = mp_void;
};

template<template<class...> class L, class T1, class... T>
struct mp_pop_front_impl<L<T1, T...> >
{
  using type = L<T...>;
};

template<class L> using mp_pop_front = typename mp_pop_front_impl<L>::type;


// mp_push_front
template<class L, class... T> struct mp_push_front_impl;

template<template<class...> class L, class... U, class... T>
struct mp_push_front_impl<L<U...>, T...>
{
  using type = L<T..., U...>;
};

template<class L, class... T>
using mp_push_front = typename mp_push_front_impl<L, T...>::type;


// mp_transform
template<template<class...> class F, class... L> struct mp_transform_impl;

template<template<class...> class F, class... L>
using mp_transform = typename mp_transform_impl<F, L...>::type;

template<template<class...> class F, template<class...> class L, class... T>
struct mp_transform_impl<F, L<T...>>
{
  using type = L<F<T>...>;
};

template<template<class...> class F,
         template<class...> class L1, class... T1,
         template<class...> class L2, class... T2>
struct mp_transform_impl<F, L1<T1...>, L2<T2...>>
{
  static_assert(sizeof...(T1) == sizeof...(T2), "");
  using type = L1<F<T1,T2>...>;
};


// mp_transform (v2)
/*
template<template<class...> class F, class E, class... L>
struct mp_transform_impl;

template<template<class...> class F, class... L>
using mp_transform = typename mp_transform_impl<F, mp_empty<L...>, L...>::type;

template<template<class...> class F, class L1, class... L>
struct mp_transform_impl<F, mp_true, L1, L...>
{
  using type = mp_clear<L1>;
};

template<template<class...> class F, class... L>
struct mp_transform_impl<F, mp_false, L...>
{
  using _first = F<mp_front<L>...>;
  using _rest = mp_transform<F, mp_pop_front<L>...>;
  using type = mp_push_front<_rest, _first>;
};
*/


// mp_constant
template<class V> struct mp_constant
{
    template<class...> using apply = V;
};


// mp_fill
template<class L, class V>
    using mp_fill = mp_transform<mp_constant<V>::template apply, L>;


// mp_from_sequence
template<class S> struct mp_from_sequence_impl;

template<template<class T, T... I> class S, class U, U... J>
struct mp_from_sequence_impl<S<U, J...>>
{
  using type = mp_list<std::integral_constant<U, J>...>;
};

template<class S> using mp_from_sequence = typename mp_from_sequence_impl<S>::type;


// mp_iota
template<class N> using mp_iota = mp_from_sequence<std::make_index_sequence<N::value>>;
template<std::size_t N> using mp_iota_v = mp_from_sequence<std::make_index_sequence<N>>;


// mp_append
/*
template<class... L> struct mp_append_impl;

template<class... L> using mp_append = typename mp_append_impl<L...>::type;

template<> struct mp_append_impl<>
{
  using type = mp_list<>;
};

template<template<class...> class L, class... T> struct mp_append_impl<L<T...>>
{
  using type = L<T...>;
};

template<template<class...> class L1, class... T1,
         template<class...> class L2, class... T2, class... Lr>
struct mp_append_impl<L1<T1...>, L2<T2...>, Lr...>
{
  using type = mp_append<L1<T1..., T2...>, Lr...>;
};
*/
template<class... L> using mp_append = brigand::append<L...>;


// mp_sequence
template<class L> using mp_sequence = mp_iota<mp_size<L>>;


// mp_tuple_cat
template<class R, class...Is, class... Js, class Tp>
R mp_tuple_cat_impl(mp_list<Is...>, mp_list<Js...>, Tp tp)
{
  return R{mp_get_v<Js::value>(mp_get_v<Is::value>(tp))...};
}

template<class... Tp, class R = mp_append<mp_tuple<>, typename std::decay<Tp>::type...> >
R mp_tuple_cat(Tp &&... tp)
{
  std::size_t const N = sizeof...(Tp);
  // list of N lists of types
  using list1 = mp_list<mp_rename<typename std::decay<Tp>::type, mp_list>...>;
  // list of N ICs from 0..N
  using list2 = mp_iota_v<N>;
  // list of N lists of ICs, i.e. [0,...][1,...]...
  using list3 = mp_transform<mp_fill, list1, list2>;
  // concatenate lists in list3
  using inner = mp_rename<mp_apply<mp_append, list3>, mp_list>;
  // list of N lists of [0,1,...][0,1,...]...
  using list4 = mp_transform<mp_sequence, list1>;
  // concatenate lists in list4
  using outer = mp_apply<mp_append, list4>;
  return mp_tuple_cat_impl<R>(inner(), outer(), mp_tuple<Tp...>(std::forward<Tp>(tp)...));
}


// mp_plus
template<class... T> struct mp_plus_impl;

template<class... T> using mp_plus = typename mp_plus_impl<T...>::type;

template<> struct mp_plus_impl<>
{
  using type = std::integral_constant<int, 0>;
};

template<class T1, class... T> struct mp_plus_impl<T1, T...>
{
  static constexpr auto _v = T1::value + mp_plus<T...>::value;

  using type = std::integral_constant<
    typename std::remove_const<decltype(_v)>::type, _v>;
};


// mp_count
template<class L, class V> struct mp_count_impl;

template<template<class...> class L, class... T, class V>
struct mp_count_impl<L<T...>, V>
{
  using type = mp_plus<std::is_same<T, V>...>;
};

template<class L, class V> using mp_count = typename mp_count_impl<L, V>::type;


// mp_bool
template<bool v> using mp_bool = std::integral_constant<bool, v>;


// mp_count_if
template<class L, template<class...> class P> struct mp_count_if_impl;

template<template<class...> class L, class... T, template<class...> class P>
struct mp_count_if_impl<L<T...>, P>
{
  using type = mp_plus<mp_bool<P<T>::value != 0>...>;
};

template<class L, template<class...> class P>
using mp_count_if = typename mp_count_if_impl<L, P>::type;


// mp_contains
template<class L, class V> using mp_contains = mp_bool<mp_count<L, V>::value != 0>;


// mp_if
template<bool C, class T, class E> struct mp_if_c_impl;

template<class T, class E> struct mp_if_c_impl<true, T, E>
{
  using type = T;
};

template<class T, class E> struct mp_if_c_impl<false, T, E>
{
  using type = E;
};

template<bool C, class T, class E>
using mp_if_c = typename mp_if_c_impl<C, T, E>::type;

template<class C, class T, class E>
using mp_if = typename mp_if_c_impl<C::value != 0, T, E>::type;


// mp_find
template<int I, typename T, typename L>
struct mp_find_impl;

template<int I, typename T, template<class...> class L>
struct mp_find_impl<I, T, L<> >
{
  static constexpr auto value = I;
};

template<int I, template<class...> class L, typename T, typename... Ts>
struct mp_find_impl<I, T, L<T, Ts...> >
{
  static constexpr auto value = I - (1 + sizeof...(Ts));
};

template<int I, typename T, template<class...> class L, typename U, typename... Ts>
struct mp_find_impl<I, T, L<U, Ts...> >
  : mp_find_impl<I, T, L<Ts...> >
{};

template<class T, class L> using mp_find = mp_find_impl<mp_size<L>::value, T, L>;


// mp_repeat_c
template<std::size_t N, class... T> struct mp_repeat_c_impl
{
  using _l1 = typename mp_repeat_c_impl<N/2, T...>::type;
  using _l2 = typename mp_repeat_c_impl<N%2, T...>::type;

  using type = mp_append<_l1, _l1, _l2>;
};

template<class... T> struct mp_repeat_c_impl<0, T...>
{
  using type = mp_list<>;
};

template<class... T> struct mp_repeat_c_impl<1, T...>
{
  using type = mp_list<T...>;
};

template<std::size_t N, class... T> using mp_repeat_c =
  typename mp_repeat_c_impl<N, T...>::type;


// mp_first
template<class L> struct mp_first_impl;

template<template<class...> class L, class T1, class... T>
struct mp_first_impl<L<T1, T...> >
{
  using type = T1;
};

template<class L> using mp_first = typename mp_first_impl<L>::type;


// mp_second
template<class L> struct mp_second_impl;

template<template<class...> class L, class T1, class T2, class... T>
struct mp_second_impl<L<T1, T2, T...> >
{
  using type = T2;
};

template<class L> using mp_second = typename mp_second_impl<L>::type;


// mp_inherit
template<class... T> struct mp_inherit : T... {};


// mp_map_find
template<class M, class K> struct mp_map_find_impl;

template<class M, class K>
using mp_map_find = typename mp_map_find_impl<M, K>::type;

template<template<class...> class M, class... T, class K>
struct mp_map_find_impl<M<T...>, K>
{
  using U = mp_inherit<mp_identity<T>...>;

  template<template<class...> class L, class... U>
  static mp_identity<L<K, U...> > f(mp_identity<L<K, U...> >*);

  static mp_identity<void> f(...);
  using V = decltype(f((U*)0));
  using type = typename V::type;
};


// mp_map_from_list
template<class L, class S> struct mp_map_from_list_impl;

template<template<class...> class L, class... T, std::size_t... J>
struct mp_map_from_list_impl<L<T...>, std::integer_sequence<std::size_t, J...> >
{
  using type = mp_list<mp_list<mp_size_t<J>, T>...>;
};

template<class L> using mp_map_from_list =
  typename mp_map_from_list_impl<L, std::make_index_sequence<mp_size<L>::value> >::type;


// mp_reversed_map_from_list
template<class L, class S> struct mp_reversed_map_from_list_impl;

template<template<class...> class L, class... T, std::size_t... J>
struct mp_reversed_map_from_list_impl<L<T...>, std::integer_sequence<std::size_t, J...> >
{
  using type = mp_list<mp_list<T, mp_size_t<J> >...>;
};

template<class L> using mp_reversed_map_from_list =
  typename mp_reversed_map_from_list_impl<L, std::make_index_sequence<mp_size<L>::value> >::type;


// mp_at
/*
template<class L, std::size_t I> struct mp_at_c_impl
{
  using map = mp_map_from_list<L>;
  using type = mp_second<mp_map_find<map, mp_size_t<I> > >;
};

template<class L, std::size_t I> using mp_at_c = typename mp_at_c_impl<L, I>::type;

template<class L, class I> using mp_at = typename mp_at_c_impl<L, I::value>::type;
*/
template<class L, std::size_t I> using mp_at_c = brigand::at_c<L, I>;

template<class L, class I> using mp_at = brigand::at<L, I>;


// mp_at (standard version)
/*
template<class L, class L2> struct mp_at_c_impl;

template<template<class...> class L, class... T,
         template<class...> class L2, class... U>
struct mp_at_c_impl<L<T...>, L2<U...>>
{
  template<class W> static W f( U*..., W*, ... );

  using R = decltype( f( (mp_identity<T>*)0 ... ) );

  using type = typename R::type;
};

template<class L, std::size_t I> using mp_at_c =
  typename mp_at_c_impl<L, mp_repeat_c<I, void>>::type;

template<class L, class I> using mp_at = mp_at_c<L, I::value>;
*/


// mp_drop
template<class L, class L2> struct mp_drop_c_impl;

template<template<class...> class L, class... T,
         template<class...> class L2, class... U>
struct mp_drop_c_impl<L<T...>, L2<U...> >
{
  template<class... W> static mp_identity<L<W...> > f(U*..., mp_identity<W>*...);
  using R = decltype(f(((mp_identity<T>*)0)...));
  using type = typename R::type;
};

template<class L, std::size_t N> using mp_drop_c =
  typename mp_drop_c_impl<L, mp_repeat_c<N, void> >::type;

template<class L, class N> using mp_drop = mp_drop_c<L, N::value>;


#endif // _mp_functions_basic_h_




// -*- C++ -*-

#ifndef _mp_functions_extended_h_
#define _mp_functions_extended_h_


// mp_type_of
template<class T> struct mp_type_of
{
  using type = typename T::type;
};

template<class T> using mp_type_of_t = typename mp_type_of<T>::type;


// mp_sizeof
template<class T> struct mp_sizeof
{
  using type = std::integral_constant<std::size_t, sizeof(T)>;
};

template<class T> using mp_sizeof_t = typename mp_sizeof<T>::type;


// mp_reverse
/*
template<class L>
struct mp_reverse_impl;

template<template<class...> class L>
struct mp_reverse_impl<L<>>
{
  using type = L<>;
};

template<template<class...> class L, typename T1, typename... T>
struct mp_reverse_impl<L<T1, T...>>
{
  using _head = L<T1>;
  using _tail = typename mp_reverse_impl<L<T...> >::type;
  using type = mp_append<_tail, _head>;
};

template<class L> using mp_reverse = typename mp_reverse_impl<L>::type;
*/
template<class L> using mp_reverse = brigand::reverse<L>;


// mp_get
template<std::size_t N, class T, class... Args>
struct mp_get_impl
{
  static constexpr auto value = N;
};

template<std::size_t N, class T, class... Args>
struct mp_get_impl<N, T, T, Args...>
{
  static constexpr auto value = N;
};

template<std::size_t N, class T, class U, class... Args>
struct mp_get_impl<N, T, U, Args...>
{
  static constexpr auto value = mp_get_impl<N + 1, T, Args...>::value;
};

template<class T, class... Args>
T mp_get(const mp_tuple<Args...>& t)
{
  std::size_t constexpr I = mp_get_impl<0, T, Args...>::value;
  return mp_get_v<I>(t);
}


// mp_get_left
template<std::size_t I>
struct mp_get_left
{
  template<class T,
           template<class...> class L0, class... T0s,
           template<class...> class L1, class... T1s>
  static T get(L0<T0s...> l0, L1<T1s...> l1)
  {
    std::size_t constexpr I0 = I;
    return mp_get_v<I0>(l0);
  }
};


// mp_get_right
template<std::size_t I>
struct mp_get_right
{
  template<class T,
           template<class...> class L0, class... T0s,
           template<class...> class L1, class... T1s>
  static T get(L0<T0s...> l0, L1<T1s...> l1)
  {
    std::size_t constexpr I0 = mp_get_impl<0, T, T1s...>::value;
    return mp_get_v<I0>(l1);
  }
};


// mp_get_pair
template<class T, class... T0s, class... T1s>
T mp_get_pair(mp_tuple<T0s...> t0, mp_tuple<T1s...> t1)
{
  std::size_t constexpr I0 = mp_get_impl<0, T, T0s...>::value;
  std::size_t constexpr N0 = sizeof...(T0s);
  return mp_if_c<I0 != N0, mp_get_left<I0>, mp_get_right<0> >::template get<T>(t0, t1);
}


// mp_merge
template<class L1, class L2>
struct mp_merge_impl;

template<class L1, class L2> using mp_merge = typename mp_merge_impl<L1, L2>::type;

template<template<class...> class L1, class... T1, template<class...> class L2, class... T2>
struct mp_merge_impl<L1<T1...>, L2<T2...> >
{
  using type = L1<mp_append<T1, T2>... >;
};


// mp_flatten
/*
template<class L>
struct mp_flatten_impl;

template<template<class...> class L, class... T>
struct mp_flatten_impl<L<T...> >
{
  using type = mp_append<T...>;
};

template<class L> using mp_flatten = typename mp_flatten_impl<L>::type;
*/
template<class L> using mp_flatten = brigand::flatten<L>;


// mp_match
template<class T> struct mp_match { using type = T; };
template<class T> using mp_match_t = typename mp_match<T>::type;


// mp_find_map_key
template<int I, typename T, typename L, typename H>
struct mp_find_map_key_impl;

template<int I, typename T, template<class...> class L, template<class...> class H, typename... HIs>
struct mp_find_map_key_impl<I, T, L<>, H<HIs...> >
{
  static constexpr auto value = I;
  using type = H<HIs...>;
};

template<int I, template<class...> class L, typename T, typename... Us, typename... Ts, template<class...> class H, typename... HIs>
struct mp_find_map_key_impl<I, T, L<mp_tuple<mp_identity<T>, Us...>, Ts...>, H<HIs...> >
{
  static constexpr auto value = I - (1 + sizeof...(Ts));
  using type = mp_append<H<HIs...>, L<mp_tuple<mp_identity<mp_match<T> >, Us...>, Ts...> >;
};

template<int I, typename T, template<class...> class L, typename U, typename... Us, typename... Ts, template<class...> class H, typename... HIs>
struct mp_find_map_key_impl<I, T, L<mp_tuple<mp_identity<U>, Us...>, Ts...>, H<HIs...> >
  : mp_find_map_key_impl<I, T, L<Ts...>, mp_append<H<HIs...>, L<mp_tuple<mp_identity<U>, Us...> > > >
{};

template<class T, class L> using mp_find_map_key = mp_find_map_key_impl<mp_size<L>::value, T, L, mp_list<> >;


// mp_insert_all
template<class S, class L> struct mp_insert_all_impl;

template<class S, class L> using mp_insert_all = typename mp_insert_all_impl<S, L>::type;

template<class S, template<class...> class L> struct mp_insert_all_impl<S, L<> >
{
  using type = S;
};

template<class S, template<class...> class L, class T1, class... T>
struct mp_insert_all_impl<S, L<T1, T...> >
{
  using _S1 = brigand::insert<S, T1>;
  using type = mp_insert_all<_S1, L<T...> >;
};


// mp_uniq_merge
template<class L1, class L2>
struct mp_uniq_merge_impl;

template<class L1, class L2> using mp_uniq_merge = typename mp_uniq_merge_impl<L1, L2>::type;

template<template<class...> class L1, class... T1, template<class...> class L2, class... T2>
struct mp_uniq_merge_impl<L1<T1...>, L2<T2...> >
{
  using type = L1<brigand::as_list<mp_insert_all<brigand::as_set<T1>, brigand::as_set<T2> > >... >;
};


// mp_resize
template<std::size_t N, class L, class T>
struct mp_resize_lazy_impl
{
  using type = mp_append<L, mp_fill<mp_iota_v<N>, T> >;
};

template<std::size_t N, class L, class T>
struct mp_resize_impl
{
  static std::size_t constexpr _NL = {mp_size<L>::value};
  static std::size_t constexpr _M = {N - _NL};
  using type = typename mp_if_c<bool(N > _NL), mp_resize_lazy_impl<_M, L, T>, mp_identity<L> >::type;
};

template<std::size_t N, class L, class T> using mp_resize = typename mp_resize_impl<N, L, T>::type;


// mp_select
template<template<class...> class LR, class... TR, template<class...> class L, class... T>
LR<TR...> mp_select_fn_impl(LR<TR...> lr, L<T...> l)
{
  return LR<TR...>{mp_get<TR>(l)...};
}

template<class R, class L>
R mp_select_fn(R r, L l)
{
  return mp_select_fn_impl(r, l);
}


// mp_cumulative
template<class L, class S>
struct mp_cumulative_impl;

template<class L, template<class _SX, _SX...> class S, class ST, ST... SVs>
struct mp_cumulative_impl<L, S<ST, SVs...> >
{
  using LR = mp_reverse<L>;
  using LC = mp_list<std::integral_constant<std::size_t, mp_apply<mp_plus, mp_drop_c<LR, SVs> >::value>...>;
  using type = mp_reverse<LC>;
};

template<class L>
struct mp_cumulative_entry
{
  static auto constexpr v = mp_size<L>::value;
  using S = std::make_index_sequence<v>;
  using LS = mp_transform<mp_size, L>;
  using type = typename mp_cumulative_impl<LS, S>::type;
};

template<class L> using mp_cumulative = typename mp_cumulative_entry<L>::type;


// mp_subgroup_find
template<class M, class L, class C, std::size_t NI> struct mp_subgroup_find_impl;

template<template<class...> class M, class L, class C, std::size_t NI>
struct mp_subgroup_find_impl<M<>, L, C, NI>
{
  using type = mp_list<>;
};

template<template<class...> class M, class... Us, class L, class C, std::size_t NI>
struct mp_subgroup_find_impl<M<mp_void, Us...>, L, C, NI>
{
  using _head = std::integral_constant<std::size_t, NI>;
  using _tail = typename mp_subgroup_find_impl<M<Us...>, L, C, NI>::type;
  using type = mp_append<mp_list<_head>, _tail>;
};

template<template<class...> class M, class U, class... Us, class L, class C, std::size_t NI>
struct mp_subgroup_find_impl<M<U, Us...>, L, C, NI>
{
  auto static constexpr depth = U::value;
  using SL = mp_at_c<L, depth>;

  auto static constexpr base = mp_at_c<C, depth>::value;
  auto static constexpr position = mp_find<U, SL>::value;
  auto static constexpr offset = std::size_t{base + position};

  using _head = std::integral_constant<std::size_t, offset>;
  using _tail = typename mp_subgroup_find_impl<M<Us...>, L, C, NI>::type;
  using type = mp_append<mp_list<_head>, _tail>;
};

template<class M, class L>
struct mp_subgroup_find_entry
{
  static auto constexpr NI = mp_size<mp_flatten<L> >::value;
  using LE = mp_push_front<L, mp_list<> >;
  using C = mp_cumulative<LE>;
  using type = typename mp_subgroup_find_impl<M, L, C, NI>::type;
};

template<class M, class L> using mp_subgroup_find = typename mp_subgroup_find_entry<M, L>::type;


// mp_incr
template<typename T0, typename T1>
using mp_incr = brigand::bool_<(T0::value < T1::value)>;


// mp_map_incr
template<typename T0, typename T1>
using mp_map_incr = brigand::bool_<(mp_first<T0>::value < mp_first<T1>::value)>;


// mp_dual
template<class L, class S, std::size_t I, std::size_t NI, std::size_t NJ>
struct mp_dual_impl;

template<template<class...> class L,
         template<class SX, SX...> class S, class ST, ST... SVs,
         std::size_t I, std::size_t NI, std::size_t NJ>
struct mp_dual_impl<L<>, S<ST, SVs...>, I, NI, NJ>
{
  using type = mp_list<>;
};

template<template<class...> class L,
         template<class SX, SX...> class S, class ST, ST SV, ST... SVs,
         std::size_t I, std::size_t NI, std::size_t NJ>
struct mp_dual_impl<L<>, S<ST, SV, SVs...>, I, NI, NJ>
{
  using _head = std::integral_constant<std::size_t, NJ>;
  using _tail = typename mp_dual_impl<L<>, S<ST, SVs...>, I + 1, NI, NJ>::type;
  using type = mp_append<mp_list<_head>, _tail>;
};


template<class L, class S, std::size_t I, std::size_t NI, std::size_t NJ, std::size_t J>
struct mp_dual_impl_valid;

template<template<class...> class L, class T, class... Ts,
         template<class _SX, _SX...> class S, class ST, ST SV, ST... SVs,
         std::size_t I, std::size_t NI, std::size_t NJ, std::size_t J>
struct mp_dual_impl_valid<L<T, Ts...>, S<ST, SV, SVs...>, I, NI, NJ, J>
{
  auto static constexpr JD = (I == J)? mp_second<T>::value: NJ;
  using _head = std::integral_constant<std::size_t, JD>;

  using _tail = typename mp_if_c<bool(I == J),
                                 mp_dual_impl<L<Ts...>, S<ST, SVs...>, I + 1, NI, NJ>,
                                 mp_dual_impl<L<T, Ts...>, S<ST, SVs...>, I + 1, NI, NJ>
                                 >::type;

  using type = mp_append<mp_list<_head>, _tail>;
};


template<class L, class S, std::size_t I, std::size_t NI, std::size_t NJ>
struct mp_dual_impl_invalid;

template<template<class...> class L, class T, class... Ts,
         template<class _SX, _SX...> class S, class ST, ST SV, ST... SVs,
         std::size_t I, std::size_t NI, std::size_t NJ>
struct mp_dual_impl_invalid<L<T, Ts...>, S<ST, SV, SVs...>, I, NI, NJ>
{
  using _head = std::integral_constant<std::size_t, NJ>;
  using _tail = typename mp_dual_impl<L<Ts...>, S<ST, SVs...>, I + 1, NI, NJ>::type;
  using type = mp_append<mp_list<_head>, _tail>;
};


template<template<class...> class L, class T, class... Ts,
         template<class _SX, _SX...> class S, class ST, ST SV, ST... SVs,
         std::size_t I, std::size_t NI, std::size_t NJ>
struct mp_dual_impl<L<T, Ts...>, S<ST, SV, SVs...>, I, NI, NJ>
{
  auto static constexpr J = mp_first<T>::value;

  using type = typename mp_if_c<bool(J < NI),
    mp_dual_impl_valid<L<T, Ts...>, S<ST, SV, SVs...>, I, NI, NJ, J>,
    mp_dual_impl_invalid<L<T, Ts...>, S<ST, SV, SVs...>, I, NI, NJ>
    >::type;
};

template<typename L, std::size_t NI>
struct mp_dual_entry
{
  using IS = std::make_index_sequence<NI>;
  auto static constexpr NJ = mp_size<L>::value;
  using M = mp_reversed_map_from_list<L>;
  using MS = brigand::sort<M, brigand::bind<mp_map_incr, brigand::_1, brigand::_2> >;
  using D = mp_dual_impl<MS, IS, 0, NI, NJ>;
  using type = typename D::type;
};

template<class L, std::size_t NI> using mp_dual = typename mp_dual_entry<L, NI>::type;


// mp_swap (if L < R)
template<class L, class R, bool swap>
struct mp_swap_impl
{
  using Left = L;
  using Right = R;
  inline static Left left(L const &l, R const &r) { return l; }
  inline static Right right(L const &l, R const &r) { return r; }
};

template<class L, class R>
struct mp_swap_impl<L, R, true>
{
  using Left = R;
  using Right = L;
  inline static Left left(L const &l, R const &r) { return r; }
  inline static Right right(L const &l, R const &r) { return l; }
};

template<class L, class R>
struct mp_swap
{
  static bool constexpr _swap = mp_size<L>::value < mp_size<R>::value;
  // static bool constexpr _swap = false;
  using _impl = mp_swap_impl<L, R, _swap>;
  using Left = typename _impl::Left;
  using Right = typename _impl::Right;
  inline static Left left(L const &l, R const &r) { return _impl::left(l, r); }
  inline static Right right(L const &l, R const &r) { return _impl::right(l, r); }
};


// mp_get_if
template<bool c, std::size_t i>
struct mp_get_if
{
  template<typename L, typename T>
  AlwaysInline inline static T &apply(L &l, T &) { return mp_get_r<i>(l); }
};

template<std::size_t i>
struct mp_get_if<false, i>
{
  template<typename L, typename T>
  AlwaysInline inline static T &apply(L &, T &nul) { return nul; }
};

#endif // _mp_functions_extended_h_
