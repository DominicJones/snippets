// Memory is automtically freed when the collection
// of class instances goes out of scope.

#include <iostream>
#include <vector>
#include <typeinfo>
#include <memory>
#include <cassert>

struct Base
{
  virtual ~Base() {}
  size_t eval() { return typeid(*this).hash_code(); }
};

struct Derived : Base {};

int main()
{
  std::vector<std::unique_ptr<Base>> pc;
  pc.emplace_back(new Derived);
  assert(pc[0]->eval() == typeid(Derived).hash_code());
}
