#include <tuple>
#include <utility>
#include <type_traits>
#include <functional>
#include <iostream>
#include <cstring>


template<std::size_t... I>
inline constexpr auto make_cref_tuple_impl(std::index_sequence<I...>, auto const &tp) noexcept
{
  return std::make_tuple(std::cref(std::get<I>(tp))...);
}

template<typename... T>
inline constexpr auto make_cref_tuple(std::tuple<T...> const &tp) noexcept
{
  return make_cref_tuple_impl(std::make_index_sequence<sizeof...(T)>(), tp);
}


template<std::size_t... I>
inline constexpr auto make_ref_tuple_impl(std::index_sequence<I...>, auto &tp) noexcept
{
  return std::make_tuple(std::ref(std::get<I>(tp))...);
}

template<typename... T>
inline constexpr auto make_ref_tuple(std::tuple<T...> &tp) noexcept
{
  return make_ref_tuple_impl(std::make_index_sequence<sizeof...(T)>(), tp);
}


template<std::size_t... I>
inline constexpr auto tuple_zip_cref_ref_impl(std::index_sequence<I...>, auto u, auto v) noexcept
{
  return std::tuple_cat(std::make_tuple(std::cref(std::get<I>(u)), std::ref(std::get<I>(v)))...);
}

template<typename... U, typename... V>
inline constexpr auto tuple_zip_cref_ref(std::tuple<U...> u, std::tuple<V...> v) noexcept
{
  return tuple_zip_cref_ref_impl(std::make_index_sequence<sizeof...(U)>(), u, v);
}


void multiply(int const &a, long &a_drv, float const &b, double &b_drv, double rhs)
{
  a_drv += b * rhs;
  b_drv += a * rhs;
}

template<std::size_t... I>
void multiply_impl(std::index_sequence<I...>, auto args, double rhs)
{
  multiply(std::get<I>(args)..., rhs);
}

template<typename... T>
void multiply(std::tuple<T...> args, double rhs)
{
  multiply_impl(std::make_index_sequence<sizeof...(T)>{}, args, rhs);
}


int main()
{
  auto aa = std::make_tuple(int{1}, float{20});
  auto aa_cref = make_cref_tuple(aa);

  std::get<0>(aa) += 1;
  std::cout << std::get<0>(aa_cref) << std::endl;

  auto aa_drv = std::make_tuple(long{300}, double{4000});
  auto aa_drv_ref = make_ref_tuple(aa_drv);

  std::get<0>(aa_drv) += 1;
  std::cout << std::get<0>(aa_drv_ref) << std::endl;

  std::get<0>(aa_drv_ref) += 1;
  std::cout << std::get<0>(aa_drv_ref) << std::endl;


  std::memset(&aa_drv, 0, sizeof(aa_drv));
  auto aa_zip = tuple_zip_cref_ref(aa_cref, aa_drv_ref);
  multiply(aa_zip, double{1});
  std::cout << std::get<0>(aa_drv) << std::endl;
  std::cout << std::get<1>(aa_drv) << std::endl;
}
