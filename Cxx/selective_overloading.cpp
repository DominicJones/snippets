#include <iostream>


namespace Default
{
  struct VectorBase {};
  auto operator+(Default::VectorBase, Default::VectorBase);
}


struct Vector : Default::VectorBase {};


namespace Default
{
  auto operator+(Default::VectorBase, Default::VectorBase)
  {
    std::cout << "Default::operator+" << std::endl;
    return Vector{};
  }
}


namespace Specialized
{
  struct Add;
  template<typename OP, typename L, typename R> struct Binary {};

  template<typename V1, typename V2>
  auto operator+(V1 &&v1, V2 &&v2)
  {
    std::cout << "Specialized::operator+" << std::endl;
    return Binary<Add, decltype(v1), decltype(v2)>{};
  }
}


int main()
{
  // comment out to use default operator overloading
  using namespace Specialized;

  Vector x;
  auto y = x + x;
}
