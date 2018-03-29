class Parent
{};


class Child
  : public Parent
{};


class Base
{
public:
  virtual Parent &getChild() = 0;
};


class Derived
  : public Base
{
public:
  Derived(Child &child)
    : _child(child)
  {}

  Child &getChild()
  {
    return _child;
  }

private:
  Child &_child;
};


#include <iostream>

int main()
{
  Child child;
  Base * base = new Derived(child);
  Parent &child_ = base->getChild();
  std::cout << (&child == &child_) << std::endl;
}
