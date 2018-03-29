#include <array>
#include <utility>
#include <type_traits>
#include <typeinfo>
#include <iostream>


using Scale = std::integer_sequence<int, 1, 2>;


template<int scale>
int eval(int value)
{
  return value * scale;
}


int evaluate(int scale, int value)
{
  using EvalFn = std::add_pointer<int (int)>::type;
  static std::array<EvalFn, Scale::size()> const evalfn = { eval<1>, eval<2> };  // expand automatically

  int const offset(scale - 1);
  return evalfn[offset](value);
}


int main()
{
  std::cout << evaluate(1, 4) << std::endl;
  std::cout << evaluate(2, 4) << std::endl;
}
