#include <memory>
#include <iostream>

struct Base
{
  Base(std::auto_ptr<int> val)
    : _val(val)
  {}

  int const * const val() const { return _val.get(); }
  std::auto_ptr<int> const _val;
};


struct Derived: Base
{
  Derived(std::auto_ptr<int> val)
    : Base(val)
  {}
};


int main()
{
  Derived used(std::auto_ptr<int>(new int(10)));
  std::cout << *used.val() << std::endl;

  Derived empty(std::auto_ptr<int>(0));
  std::cout << *empty.val() << std::endl;
}
