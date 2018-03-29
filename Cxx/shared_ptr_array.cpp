#include <iostream>
#include <vector>
#include <tr1/memory>

class Container
{
public:
  Container(double const * const value): _value(value) {}
  ~Container() { delete _value; }
  double value() const { return *_value; }
private:
  double const * const _value;
};

typedef std::vector<std::tr1::shared_ptr<Container> > Collection;

Collection generate()
{
  Collection collection;
  typedef Collection::value_type ManagedItem;
  typedef ManagedItem::element_type Item;
  collection.push_back(ManagedItem(new Item(new double(123))));
  return collection;
}

int main()
{
  Collection collection(generate());
  std::cout << collection[0]->value() << std::endl;
}
