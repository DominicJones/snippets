// taken from:
// http://coliru.stacked-crooked.com/a/e81b661322d4e4e6

#include <tuple>
#include <type_traits>


// quote
template<template<typename...> class F>
struct quote
{
  template<typename... L> using apply = F<L...>;
};


// project_type
template<class F>
struct project_type
{
  template<class... L> using apply = typename F::template apply<typename L::type...>;
};


// typelist
template<typename... L>
struct typelist
{
  static constexpr std::size_t size = sizeof...(L);
};

template<template<class...> class T, class... L>
typelist<L...> as_typelist_fn(typelist<T<L...> >);

template<class L>
using as_typelist = decltype(as_typelist_fn(typelist<L>{}));


// apply
template<typename F, typename L>
struct apply;

template<typename F, typename... L>
struct apply<F, typelist<L...> >
{
  using type = typename F::template apply<L...>;
};

template<typename F, typename L>
using apply_t = typename apply<F, L>::type;


// not_
template<typename F>
struct not_
{
  template<typename T>
  using apply = std::conditional_t<F::template apply<T>::value, std::false_type, std::true_type>;
};


// join
template<typename... L>
struct join;

template<typename T>
struct join<T>
{
  using type = T;
};

template<typename... L1, typename... L2, typename... L3>
struct join<typelist<L1...>, typelist<L2...>, L3...>
{
  using type = typename join<typelist<L1..., L2...>, L3...>::type;
};

template<typename... L>
using join_t = typename join<L...>::type;


// filter
template<typename T, typename F>
using filter_each = std::conditional_t<F::template apply<T>::value, typelist<T>, typelist<> >;

template<typename F, typename... L>
join_t<filter_each<L, F>...> filter_fn(typelist<L...>);

template<typename L, typename F>
using filter_t = decltype(filter_fn<F>(L{}));


// partition
template<typename L, typename F>
struct partition
{
  using type = join_t<filter_t<L, F>, filter_t<L, not_<F> > >;
};

template<typename L, typename F>
using partition_t = typename partition<L, F>::type;


// index_map
template<class T, std::size_t I, std::size_t J>
struct index_map_each
{
  using type = T;
  static constexpr std::size_t index = I;
  static constexpr std::size_t converted_index = J;
};


template<class... L, std::size_t... Is>
typelist<index_map_each<L, Is, 0>...> build_indices_fn(typelist<L...>, std::index_sequence<Is...>);

template<class TL>
using build_indices = decltype(build_indices_fn(TL{}, std::make_index_sequence<TL::size>{}));


template<class... L, std::size_t... Is, std::size_t... Js>
typelist<index_map_each<L, Is, Js>...> build_converted_indices_fn(typelist<index_map_each<L, Is, 0>...>, std::index_sequence<Js...>);

template<class TL>
using build_converted_indices = decltype(build_converted_indices_fn(TL{}, std::make_index_sequence<TL::size>{}));


// make_index_map
template<class... L>
struct index_map : L... {};

template<class TL, class F>
using make_index_map = apply_t<quote<index_map>, build_converted_indices<  partition_t<build_indices<TL>, project_type<F> >   > >;


// converted_index
template<std::size_t I, std::size_t J, class T>
index_map_each<T, I, J> convert_index_fn(index_map_each<T, I, J>);

template<std::size_t I, class IndexMap>
using converted_index_t = decltype(convert_index_fn<I>(IndexMap{}));


template<class TL, class F>
struct tuple_partitioner
{
  using map_type = make_index_map<as_typelist<TL>, F>;

  using reordered_type = apply_t<project_type<quote<std::tuple> >, as_typelist<map_type> >;

  template<std::size_t I>
  using converted_index_for = std::integral_constant<std::size_t, converted_index_t<I, map_type>::converted_index>;
};


#include <iostream>
int main()
{
  using original_tuple = std::tuple<int, double, long, float, short>;
  using f = quote<std::is_integral>;
  using partitioner = tuple_partitioner<original_tuple, f>;
  static_assert(std::is_same<partitioner::reordered_type, std::tuple<int, long, short, double, float>>{}, "");

  static_assert(partitioner::converted_index_for<0>() == 0, "");
  static_assert(partitioner::converted_index_for<1>() == 3, "");
  static_assert(partitioner::converted_index_for<2>() == 1, "");
  static_assert(partitioner::converted_index_for<3>() == 4, "");
  static_assert(partitioner::converted_index_for<4>() == 2, "");
}
