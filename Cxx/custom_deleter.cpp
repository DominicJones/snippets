#include <iostream>
#include <memory>


struct Foo
{
  Foo(int &value) : value(value) {}
  ~Foo() {}

private:
  int &value;
};


template<class Deleter>
struct D
{
  D(int &value) : value(value) {};

  void operator()(Foo * p) const
  {
    value = 1;
    Deleter()(p);
  }

private:
  int &value;
};



int main()
{
  int value = 0;

  {
    using Del = D<std::default_delete<Foo>>;
    std::unique_ptr<Foo, Del> up4(new Foo(value), Del(value));
  }

  std::cout << value << std::endl;
}
