#include <iostream>


class Object
{
public:
  Object(int * const &arg)
    : _arg(arg)
  {}

  int eval() const
  {
    if (_arg)
      return *_arg;
    else
      return -1;
  }
private:
  int * const &_arg;
};


int main()
{
  int * arg = nullptr;
  Object obj(arg);
  std::cout << obj.eval() << std::endl;
  arg = new int(1);
  std::cout << obj.eval() << std::endl;
  delete arg;
}
