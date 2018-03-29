#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <array>
#include <tuple>


struct Density
{
  Density(char c, int i, double d) {}
};

struct Pressure
{
  Pressure(char c, int i, double d) {}
};


template<class Args>
struct Field;

template<class... Var>
struct Field<std::tuple<Var...> >
{
  using Vars = std::tuple<Var...>;

  template<typename... Args>
  Field(Args const &... args)
    : _vars(std::make_tuple(Var{args...}...))
  {}

  Vars const _vars;
};


int main()
{
  using Vars = std::tuple<Density, Pressure>;
  Field<Vars>('a', 1, 3.14);
}
