#include <type_traits>
#include <iostream>

class Operator {};
class Summate: Operator {};
class Multiply: Operator {};

template<class Operator> class Eval
{
public:
  template<class Operator_t = Operator>
  explicit Eval(int const &val, typename std::enable_if<std::is_same<Operator_t, Summate>::value>::type * resolve = 0)
  : _val(val + val)
  {}

  template<class Operator_t = Operator>
  explicit Eval(int const &val, typename std::enable_if<std::is_same<Operator_t, Multiply>::value>::type * resolve = 0)
  : _val(val * val)
  {}

  int result() const { return _val; }

private:
  int const _val;
};

int main()
{
  std::cout << Eval<Multiply>(3).result() << std::endl;
  std::cout << Eval<Summate>(3).result() << std::endl;
}
