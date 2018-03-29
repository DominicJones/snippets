#include <iostream>
#include <typeinfo>

class Collection
{
public:
  virtual ~Collection() {}
  template<class T> void insert() const
  {
    std::cout << "Collection::insert" << std::endl;
  }
};

class TmpCollection: public Collection
{
public:
  template<class T> void insert() const
  {
    std::cout << "TmpCollection::insert" << std::endl;
    Collection::insert<T>();
  }
};

struct TmpCollectionHelper
{
  template<class T> static void insert(Collection const * const var)
  {
    if (typeid(*var) == typeid(TmpCollection))
      {
        static_cast<TmpCollection const * const>(var)->insert<T>();
      }
    else
      {
        var->insert<T>();
      }
  }

  template<class T> static void insert(Collection const &var)
  {
    if (typeid(var) == typeid(TmpCollection))
      {
        static_cast<TmpCollection const &>(var).insert<T>();
      }
    else
      {
        var.insert<T>();
      }
  }
};

template<class T> void insert(Collection const * const var)
{
  TmpCollectionHelper::insert<T>(var);
}

template<class T> void insert(Collection const &var)
{
  TmpCollectionHelper::insert<T>(var);
}

int main()
{
  {
    std::cout << "case 1" << std::endl;
    Collection * var = new Collection();
    insert<int>(var);
  }

  {
    std::cout << "case 2" << std::endl;
    Collection * var = new TmpCollection();
    insert<int>(var);
  }

  {
    std::cout << "case 3" << std::endl;
    Collection var;
    insert<int>(var);
  }

  {
    std::cout << "case 4" << std::endl;
    TmpCollection var;
    insert<int>(var);
  }
}
