#include <iostream>


class Engine
{
public:
  void operator[](int it) const
  {
    std::cout << "c" << std::endl;
  }

  void operator[](int it)
  {
    std::cout << "m" << std::endl;
  }
};


class Field
{
public:
  void operator[](int it) const
  {
    std::cout << "C" << std::endl;
    _engine[it];
  }

  void operator[](int it)
  {
    std::cout << "M" << std::endl;
    _engine[it];
  }

private:
  mutable Engine _engine;
};


int main()
{
  Field const f;
  f[0]; // C m
}
