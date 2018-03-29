#include <memory>
#include <vector>


class Collection
{
public:
  Collection(int size)
    : _data(std::vector<double>(size))
  {}
private:
  std::vector<double> _data;
};


class Wrapper
{
public:
  Wrapper(int size)
    : _collection(std::auto_ptr<Collection>(new Collection(size)))
  {}
private:
  std::auto_ptr<Collection> const _collection;
};


int main()
{
  Wrapper w(3);
}
