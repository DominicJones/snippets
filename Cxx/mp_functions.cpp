// -*- C++ -*-

#ifndef _demangle_h_
#define _demangle_h_

#include <typeinfo>
#include <iostream>
#include <cstdlib>
#include <memory>

#if defined(__GNUG__)
#include <cxxabi.h>
struct Demangle
{
  static std::string eval(char const * name)
  {
    int status(-4);
    char * realname;

    realname = abi::__cxa_demangle(name, 0, 0, &status);

    std::string result(realname);
    free(realname);
    return result;
  }
};

template<typename Expr_t>
std::string demangle(Expr_t const &expr)
{
  return Demangle::eval(typeid(expr).name());
}

template<typename Expr_t>
std::string demangle()
{
  return Demangle::eval(typeid(Expr_t).name());
}
#endif

#endif // _demangle_h_




// -*- C++ -*-

#ifndef _mp_functions_h_
#define _mp_functions_h_

/*
Based on:
  http://www.pdimov.com/cpp2/simple_cxx11_metaprogramming.html
  http://www.pdimov.com/cpp2/simple_cxx11_metaprogramming_2.html
  by Peter Dimov, 26.05.2015
*/

#include <tuple>
#include <type_traits>
#include <utility>


// mp_identity
template<class T> struct mp_identity { using type = T; };
template<class T> struct mp_identity<mp_identity<T> > { using type = T; };

template<class T> using mp_identity_t = typename mp_identity<T>::type;


// mp_list
template<class... T> struct mp_list {};


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
mp_true — an alias for std::integral_constant<bool, true>
mp_false — an alias for std::integral_constant<bool, false>
mp_empty<L...> — returns mp_true if all lists are empty, mp_false otherwise
mp_clear<L> — returns an empty list of the same type as L
mp_front<L> — returns the first element of L
mp_pop_front<L> — returns L without its first element
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
    using _first = F< typename mp_front_impl<L>::type... >;
    using _rest = mp_transform< F, typename mp_pop_front_impl<L>::type... >;
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


// mp_sequence
template<class L> using mp_sequence = mp_iota<mp_size<L>>;


// mp_tuple_cat
template<class R, class...Is, class... Js, class Tp>
R mp_tuple_cat_impl(mp_list<Is...>, mp_list<Js...>, Tp tp)
{
  return R{std::get<Js::value>(std::get<Is::value>(tp))...};
}

template<class... Tp, class R = mp_append<std::tuple<>, typename std::decay<Tp>::type...> >
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
  return mp_tuple_cat_impl<R>(inner(), outer(), std::forward_as_tuple(std::forward<Tp>(tp)...));
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


// mp_unique
template<class L> struct mp_unique_impl;

template<class L> using mp_unique = typename mp_unique_impl<L>::type;

template<template<class...> class L> struct mp_unique_impl<L<>>
{
  using type = L<>;
};

template<template<class...> class L, class T1, class... T>
struct mp_unique_impl<L<T1, T...>>
{
  using _tail = mp_unique<L<T...>>;

  // FIX special hard coded condition...
  static bool constexpr _uniq = std::remove_pointer_t<T1>::is_unique;

  static bool constexpr _find = mp_contains<_tail, T1>::value;
  static bool constexpr _cond = (!_uniq) && _find;
  using type = mp_if_c<_cond, _tail, mp_push_front<_tail, T1>>;
};


// mp_reverse
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


// mp_at
template<class L, std::size_t I> struct mp_at_c_impl
{
  using map = mp_map_from_list<L>;
  using type = mp_second<mp_map_find<map, mp_size_t<I> > >;
};

template<class L, std::size_t I> using mp_at_c = typename mp_at_c_impl<L, I>::type;

template<class L, class I> using mp_at = typename mp_at_c_impl<L, I::value>::type;


/*
// mp_at (standard version)
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
T mp_get(const std::tuple<Args...>& t)
{
    return std::get<mp_get_impl<0, T, Args...>::value>(t);
}


// mp_stable_unique
template<class L> using mp_stable_unique = mp_reverse<mp_unique<mp_reverse<L> > >;

template<template<class...> class LR, class... TR, template<class...> class L, class... T>
LR<TR...> mp_stable_unique_fn_impl(LR<TR...> lr, L<T...> l)
{
    return {mp_get<TR>(l)...};
}

template<template<class...> class L, class... T>
mp_stable_unique<L<T...> > mp_stable_unique_fn(L<T...> l)
{
    using R = mp_stable_unique<L<T...> >;
    return mp_stable_unique_fn_impl(R{}, l);
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


// mp_expand
template<std::size_t N, class L, class T>
struct mp_expand_impl
{
    static std::size_t constexpr _M = {N - mp_size<L>::value};
    using _fill = mp_fill<mp_iota_v<_M>, T>;
    using type = mp_append<L, _fill>;
};

template<std::size_t N, class L, class T> using mp_expand = typename mp_expand_impl<N, L, T>::type;


#endif // _mp_functions_h_




#include <iostream>
#include <typeinfo>
int main()
{
  {
    static_assert(std::is_same< mp_rename<std::tuple<int, float, void*>, mp_list>, mp_list<int, float, void*> >::value, "");
    static_assert(mp_size<mp_list<int, float, void*> >::value == 3, "");
    static_assert(mp_size<std::tuple<int, float, void*> >::value == 3, "");
    static_assert(mp_apply<mp_length, std::tuple<int, float, void*> >::value == 3, "");
  }

  {
    using input = std::pair<std::tuple<int, float, void*>, std::tuple<int, float, void*>>;
    using result = mp_transform<std::pair, input::first_type, input::second_type>;
    using expected = std::tuple<std::pair<int, int>, std::pair<float, float>, std::pair<void*, void*>>;
    static_assert(std::is_same<result, expected>::value, "");
  }

  {
    std::tuple<int, short, long> t1;
    std::tuple<> t2;
    std::tuple<float, double, long double> t3;
    std::tuple<void*, char*> t4;
    using result = decltype(mp_tuple_cat(t1, t2, t3, t4));
    using expected = std::tuple<int, short, long, float, double, long double, void*, char*>;
    static_assert(std::is_same<expected, result>::value, "");
  }

  {
    using input = std::tuple<int, short, long, short>;
    using result = mp_unique<input>;
    using expected = std::tuple<int, long, short>;
    static_assert(std::is_same<expected, result>::value, "");
  }

  {  
    using T = std::tuple<float, float, int, int, float>;
    T t{1,2,3,4,5};
    auto u = mp_stable_unique_fn(t);
    std::cout << std::get<0>(u) << ", " << std::get<1>(u) << std::endl;
  }
}
