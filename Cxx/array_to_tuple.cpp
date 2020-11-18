#include <array>
#include <tuple>
#include <utility>
#include <iostream>

template<int N, typename T>
struct ArrayToTuple
{
  using type = decltype(std::tuple_cat(std::declval<std::array<T,N> >()));
};

template<int N, typename T>
using ArrayToTuple_t = typename ArrayToTuple<N, T>::type;

template<typename> struct debug;

int main()
{
  debug<ArrayToTuple_t<3, int> >{};
}
