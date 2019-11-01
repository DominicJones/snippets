#include <utility>
#include <iostream>


struct Add
{
  static bool constexpr is_trivial = true;
};

struct Mul
{
  static bool constexpr is_trivial = false;
};


template<typename Fn>
struct Operator
{
  template<typename Enable_t = void>
  static std::enable_if_t<Fn::is_trivial, Enable_t>
  eval_impl()
  {
    std::cout << "true" << std::endl;
  }

  template<typename Enable_t = void>
  static std::enable_if_t<!Fn::is_trivial, Enable_t>
  eval_impl()
  {
    std::cout << "false" << std::endl;
  }

  static void eval() { eval_impl(); }
};


int main()
{
  Operator<Add>::eval();
  Operator<Mul>::eval();
}
