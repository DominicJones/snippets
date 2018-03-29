#include <iostream>
#include <vector>


class Container
{
public:
  Container()
  {
    a = new int;
    *a = -1;
  }

  ~Container()
  {
    delete a;
  }

  int * a;
};


int main()
{
  Container * c;
  c = new Container;

  Container * p;
  p = c;
  std::cout << (*(*p).a) << std::endl;

  delete c;
  std::cout << (*(*p).a) << std::endl;
}
