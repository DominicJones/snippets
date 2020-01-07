#include <iostream>


struct Base {
  virtual ~Base() {}
};


struct Derived : Base {
  virtual void name() { std::cout << "Derived" << std::endl; }
};


int main()
{
  Base * b = new Derived;
  if(auto d = dynamic_cast<Derived*>(b))
  {
    d->name();
  }
}
