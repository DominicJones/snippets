#include <iostream>
#include <tuple>


template <std::size_t I, typename T>
struct indexed
{
  using type = T;
};

template <typename Is, typename ...Ts>
struct indexer;

template <std::size_t ...Is, typename ...Ts>
struct indexer<std::index_sequence<Is...>, Ts...>
  : indexed<Is, Ts>...
{};

template <std::size_t I, typename T>
static indexed<I, T> select(indexed<I, T>);

template <std::size_t I, typename ...Ts>
using nth_element = typename decltype(select<I>(indexer<std::index_sequence_for<Ts...>, Ts...>{}))::type;


int main()
{
  using n = nth_element<1, char, bool>;
  static_assert(std::is_same<n, bool>::value, "");
}
