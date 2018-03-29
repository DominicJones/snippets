#include <type_traits>
#include <typeinfo>
#include <iostream>

struct Operator {};
struct Summate: Operator {};
struct Multiply: Operator {};

template<class Operator> class Eval
{
public:
  template<class Operator_t>
  explicit Eval(Operator_t const &, typename std::enable_if<std::is_same<Operator, Summate>::value && std::is_same<Operator_t, Summate>::value>::type * = 0)
  {
    std::cout << typeid(Operator).name() << "::" << typeid(Operator_t).name() << std::endl;
  }

  template<class Operator_t>
  explicit Eval(Operator_t const &, typename std::enable_if<std::is_same<Operator, Multiply>::value && std::is_same<Operator_t, Multiply>::value>::type * = 0)
  {
    std::cout << typeid(Operator).name() << "::" << typeid(Operator_t).name() << std::endl;
  }
};

int main()
{
  Eval<Multiply> em((Multiply()));
  Eval<Summate> es((Summate()));
  // Eval<Summate> es_bad((Multiply()));
}
