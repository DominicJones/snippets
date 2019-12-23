#include <tuple>
#include <utility>
#include <type_traits>
#include <functional>
#include <iostream>



template<std::size_t... I>
constexpr auto tuple_zip_cref_ref_impl(std::index_sequence<I...>, auto u, auto v) noexcept
{
  return std::tuple_cat(std::make_tuple(std::cref(std::get<I>(u)), std::ref(std::get<I>(v)))...);
}

template<typename... U, typename... V>
constexpr auto tuple_zip_cref_ref(std::tuple<U...> u, std::tuple<V...> v) noexcept
{
  return tuple_zip_cref_ref_impl(std::make_index_sequence<sizeof...(U)>(), u, v);
}


auto summate(int const &i, long &l, float const &f, double &d)
{
  return i + l + f + d;
}

template<std::size_t... I>
auto eval_summate_impl(std::index_sequence<I...>, auto u)
{
  return summate(std::get<I>(u)...);
}

template<typename... U>
auto eval_summate(std::tuple<U...> u)
{
  return eval_summate_impl(std::make_index_sequence<sizeof...(U)>(), u);
}


int main()
{
  int i = 1;
  float f = 20;
  std::tuple<int const &, float const &> t1{i, f};

  long l = 300;
  double d = 4000;
  std::tuple<long &, double &> t2{l, d};
  std::cout << eval_summate(tuple_zip_cref_ref(t1, t2)) << std::endl;

  l += l;
  d += d;
  std::cout << eval_summate(tuple_zip_cref_ref(t1, t2)) << std::endl;
}
