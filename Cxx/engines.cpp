#include <memory>
#include <iostream>

using Index = int;
using Real = float;

namespace v1
{
  template<class T>
  struct VirtualEngine
  {
    virtual T operator[](Index i) const = 0;
  };

  template<class T, class E>
  struct StaticEngine : VirtualEngine<T>
  {
    T operator[](Index i) const final
    {
      return static_cast<E const &>(*this).E::at(i);
    }
  };

  struct ConcreteEngine : StaticEngine<Real, ConcreteEngine>
  {
    Real at(Index i) const { return Real(1); }
  };
}


namespace v2
{
  template<class T>
  struct VirtualEngine
  {
    using at_fn_ptr_t = T(*)(VirtualEngine const &, Index);
    virtual at_fn_ptr_t at_fn_ptr() const = 0;
    virtual T operator[](Index i) const = 0;
  };

  template<class T, class E>
  struct StaticEngine : VirtualEngine<T>
  {
    using typename VirtualEngine<T>::at_fn_ptr_t;
    T operator[](Index i) const final
    {
      return static_cast<E const &>(*this).E::at(i);
    }
    static T at_fn(VirtualEngine<T> const &e, Index i)
    {
      return static_cast<E const &>(e).E::at(i);
    }
    at_fn_ptr_t at_fn_ptr() const final
    {
      return at_fn;
    }
  };

  struct ConcreteEngine : StaticEngine<Real, ConcreteEngine>
  {
    Real at(Index i) const { return Real(2); }
  };
}


namespace v3
{
  template<class T>
  struct DispatchEngine
  {
    using at_fn_ptr_t = T(*)(DispatchEngine const &, Index);
    DispatchEngine(at_fn_ptr_t at_fn_ptr) : _at_fn_ptr(at_fn_ptr) {}
    T operator[](Index i) const
    {
      return _at_fn_ptr(*this, i);
    }
    at_fn_ptr_t _at_fn_ptr;
  };

  template<class T, class E>
  struct StaticEngine : DispatchEngine<T>
  {
    StaticEngine() : DispatchEngine<T>(at_fn) {}
    T operator[](Index i) const
    {
      return static_cast<E const &>(*this).E::at(i);
    }
    static T at_fn(DispatchEngine<T> const &e, Index i)
    {
      return static_cast<E const &>(e).E::at(i);
    }
  };

  struct ConcreteEngine : StaticEngine<Real, ConcreteEngine>
  {
    Real at(Index i) const { return Real(3); }
  };
}


int main()
{
  {
    using namespace v1;
    std::unique_ptr<VirtualEngine<Real> > e;
    e = std::make_unique<ConcreteEngine>();
    std::cout << (*e)[Index(0)] << std::endl;
  }

  {
    using namespace v2;
    std::unique_ptr<VirtualEngine<Real> > e;
    e = std::make_unique<ConcreteEngine>();
    std::cout << (*e)[Index(0)] << std::endl;
  }

  {
    using namespace v3;
    std::unique_ptr<DispatchEngine<Real> > e;
    e = std::make_unique<ConcreteEngine>();
    std::cout << (*e)[Index(0)] << std::endl;
  }
}
