#include <type_traits>
#include <utility>
#include <tuple>
#include <iostream>
#include <typeinfo>


template<std::size_t Ofst,
         class Tuple,
         std::size_t... I>
constexpr auto tuple_slice_impl(Tuple&& t, std::index_sequence<I...>)
{
  return std::forward_as_tuple(std::get<I + Ofst>(std::forward<Tuple>(t))...);
}


template<std::size_t I1,
         std::size_t I2,
         class Cont>
constexpr auto tuple_slice(Cont&& t)
{
  return tuple_slice_impl<I1>(std::forward<Cont>(t),
                              std::make_index_sequence<I2 - I1>{});
}


int main()
{
  std::tuple<char, bool, float, int, double, long> r{'0', 1, 2.0, 3, 4.0, 5};
  auto r0 = tuple_slice<2, 4>(r);
}
