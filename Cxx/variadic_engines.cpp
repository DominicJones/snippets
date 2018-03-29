#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <array>
#include <tuple>
#include <utility>

#include "integer_sequence.h"


struct Density
{
  Density(char c, int i, double d) {}
};

struct Pressure
{
  Pressure(char c, int i, double d) {}
};


template<typename Var> struct Engine
{
  template<typename... Args>
  Engine(Args const &... args)
  {}

  double operator[](int it) { return it; }
};


template<class Args>
struct Field;

template<class... Var>
struct Field<std::tuple<Var...> >
{
  using Vars = std::tuple<Var...>;
  using Engines = std::tuple<Engine<Var>...>;
  static int constexpr N = sizeof...(Var);

  template<typename... Args>
  Field(Args const &... args)
    : _vars(std::make_tuple(Var{args...}...))
    , _engines(std::make_tuple(Engine<Var>{args...}...))
  {}

  std::array<double, N> operator[](int it)
  {
    std::array<double, N> result;
    operator_index(std::make_index_sequence<N>{}, it, result);
    return result;
  }

  template<std::size_t... I>
  typename std::enable_if<bool(std::index_sequence<I...>::size() != 0)>::type
  operator_index(std::index_sequence<I...>, int it, std::array<double, N> &result) const
  {
    static int constexpr M = std::index_sequence<I...>::size();
    result[M] = std::get<M>(_engines)[it];
  }

  template<std::size_t... I>
  typename std::enable_if<bool(std::index_sequence<I...>::size() == 0)>::type
  operator_index(std::index_sequence<I...>, int it, std::array<double, N> &result) const
  {}

  Vars const _vars;
  Engines const _engines;
};


int main()
{
  using Vars = std::tuple<Density, Pressure>;
  Field<Vars>('a', 1, 3.14);
}
