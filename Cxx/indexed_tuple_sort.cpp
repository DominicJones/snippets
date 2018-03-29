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




// taken from:
// https://codereview.stackexchange.com/q/131194

#include <tuple>
#include <utility>

// swap types at index i and index j in the template argument tuple
template <std::size_t i, std::size_t j, class Tuple>
class tuple_element_swap
{
  template <class IndexSequence>
  struct tuple_element_swap_impl;

  template <std::size_t... indices>
  struct tuple_element_swap_impl<std::index_sequence<indices...>>
  {
    using type = std::tuple
      <
      std::tuple_element_t
      <
        indices != i && indices != j ? indices : indices == i ? j : i, Tuple
        >...
      >;
  };

public:
  using type = typename tuple_element_swap_impl
    <
  std::make_index_sequence<std::tuple_size<Tuple>::value>
  >::type;
};


// selection sort template argument tuple's variadic template's types
template <template <class, class> class Comparator, class Tuple>
class tuple_selection_sort
{
  // selection sort's "loop"
  template <std::size_t i, std::size_t j, std::size_t tuple_size, class LoopTuple>
  struct tuple_selection_sort_impl
  {
    // this is done until we have compared every element in the type list
    using tuple_type = std::conditional_t
      <
      Comparator
      <
        std::tuple_element_t<i, LoopTuple>,
        std::tuple_element_t<j, LoopTuple>
        >::value,
      typename tuple_element_swap<i, j, LoopTuple>::type, // true: swap(i, j)
      LoopTuple                                           // false: do nothing
      >;

    using type = typename tuple_selection_sort_impl // recurse until j == tuple_size
      <
      i, j + 1, tuple_size, tuple_type // using the modified tuple
      >::type;
  };

  template <std::size_t i, std::size_t tuple_size, class LoopTuple>
  struct tuple_selection_sort_impl<i, tuple_size, tuple_size, LoopTuple>
  {
    // once j == tuple_size, we increment i and start j at i + 1 and recurse
    using type = typename tuple_selection_sort_impl
      <
      i + 1, i + 2, tuple_size, LoopTuple
      >::type;
  };

  template <std::size_t j, std::size_t tuple_size, class LoopTuple>
  struct tuple_selection_sort_impl<tuple_size, j, tuple_size, LoopTuple>
  {
    // once i == tuple_size, we know that every element has been compared
    using type = LoopTuple;
  };

public:
  using type = typename tuple_selection_sort_impl
    <
  0, 1, std::tuple_size<Tuple>::value, Tuple
  >::type;

  tuple_selection_sort<Comparator, Tuple>::type
  operator()(Tuple const &t) const
  {
    tuple_selection_sort<Comparator, Tuple>::type u;
    return u;
  }
};


template <class T, class U>
struct descending
  : std::conditional_t<( std::remove_pointer_t<U>::value < std::remove_pointer_t<T>::value ), std::true_type, std::false_type>
{};







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



template<template<class...> class T, class... L>
std::tuple<L...> as_tuple_fn(std::tuple<T<L...> >);

template<class L>
using as_tuple = decltype(as_tuple_fn(std::tuple<L>{}));



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
struct index_map_each : std::integral_constant<std::size_t, sizeof(T)>
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

template<class TL, class TCL>
using make_index_map = apply_t<quote<index_map>, build_converted_indices<TCL> >;


// converted_index
template<std::size_t I, std::size_t J, class T>
index_map_each<T, I, J> convert_index_fn(index_map_each<T, I, J>);

template<std::size_t I, class IndexMap>
using converted_index_t = decltype(convert_index_fn<I>(IndexMap{}));



template<class TL, template<class, class> class OP>
struct tuple_sort
{
private:
  using A = build_indices<as_typelist<TL> >;
  using B = as_tuple<A>;
  using C = typename tuple_selection_sort<OP, B>::type;
  using D = as_typelist<C>;
  using E = make_index_map<as_typelist<TL>, D>;

public:
  using type = apply_t<project_type<quote<std::tuple> >, as_typelist<E> >;
  template<std::size_t I> using offset =
    std::integral_constant<std::size_t, converted_index_t<I, E>::converted_index>;
};


#include <iostream>
int main()
{
  {
    using input = std::tuple<int, double, long, float, short>;
    using engine = tuple_sort<input, descending>;
    using result = engine::type;
    std::cout << demangle<input>() << std::endl;
    std::cout << demangle<result>() << std::endl;
    std::cout << engine::offset<3>::value << std::endl;
  }
}
