#include <type_traits>
#include <typeinfo>
#include <iostream>

template<class T> struct C
{
  double operator[](int) const;
};

using F = decltype(&C<char>::operator[]);
using R = std::result_of<F(C<char> &, int)>::type;

static_assert(std::is_same<R, double>::value, "");

int main()
{
  // prints mangled form of "double (C<char>::*)(int) const"
  std::cout << typeid(decltype(&C<char>::operator[])).name() << std::endl;
}
