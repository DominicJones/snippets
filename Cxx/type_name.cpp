#include <iostream>
#include <typeinfo>

class Scalar {};

template <class T> void showTypeName()
{
  std::cout << typeid(T).name() << std::endl;
}

int main()
{
  showTypeName<double>();
  showTypeName<Scalar>();
}
