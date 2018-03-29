#include <cassert>
#include <memory>

namespace StorageHelper
{
  struct Storage { virtual ~Storage(){} };
  struct LocalStorage : virtual Storage {};
  struct NonLocalStorage : virtual Storage {};
  struct AllStorage : virtual NonLocalStorage, virtual LocalStorage {};

  template<class T> std::auto_ptr<Storage> select()
  {
    return std::auto_ptr<Storage>(new T());
  }

  template<class T> bool isSelected(Storage const * const option)
  {
    return dynamic_cast<T const * const>(option);
  }
}

int clear(StorageHelper::Storage const * const option)
{
  using namespace StorageHelper;
  int status = 0;
  if (isSelected<LocalStorage>(option))
    {
      status += 1;
    }
  if (isSelected<NonLocalStorage>(option))
    {
      status += 10;
    }
  return status;
}

int main()
{
  using namespace StorageHelper;
  assert(clear(select<LocalStorage>().get())    ==  1);
  assert(clear(select<NonLocalStorage>().get()) == 10);
  assert(clear(select<AllStorage>().get())      == 11);
}
