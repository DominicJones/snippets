// An example of deducing the return type for a
// template function
//
// Note:
// Variadic tempate arguments were considered for
// this example (i.e. to add any number of values)
// but it is not possible to iterate over variadic
// template arguments. A recursive approach must
// instead be used, but is clumsy.

#include <iostream>
#include <utility>

template <typename Op1, typename Op2>
auto add(Op1 const &op1, Op2 const &op2)
-> decltype(op1 + op2)
{
  auto res = op1 + op2;
  return std::move(res);
}

int main()
{
  float op1 = 1;
  double op2 = 2;
  auto res = add<float, double>(op1, op2);
  std::cout << res << std::endl;
}
