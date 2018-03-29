/*
  Based on:
  1. http://blog.coldflake.com/posts/2014-01-12-C%2B%2B-delegates-on-steroids.html
  2. http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
*/

#include <type_traits>
#include <functional>
#include <typeinfo>
#include <iostream>
#include <memory>


template<typename Ret, typename... Args>
class Delegate
{
  using Function = Ret (*)(void*, Args...);
  template<typename T> using Method = Ret (T::*)(Args...);
  template<typename T> using Method_const = Ret (T::*)(Args...) const;

public:
  inline Delegate()
    : _object(nullptr)
    , _function(nullptr)
  {}

  inline Delegate(void * object, Function function)
    : _object(object)
    , _function(function)
  {}

  inline bool hasBinding() const
  {
    return _object && _function;
  }

  template<typename T, Method<T> method>
  inline static Delegate bind(T * object)
  {
    return Delegate(object, &evaluate<T, method>);
  }

  template<typename T, Method_const<T> method>
  inline static Delegate bind(T * object)
  {
    return Delegate(object, &evaluate<T, method>);
  }

  inline Ret operator()(Args... args) const
  {
    return (*_function)(_object, args...);
  }

  template<typename T, Method<T> method>
  inline static Ret evaluate(void * object, Args... args)
  {
    return (static_cast<T*>(object)->*(method))(args...);
  }

  template<typename T, Method_const<T> method>
  inline static Ret evaluate(void * object, Args... args)
  {
    return (static_cast<T*>(object)->*(method))(args...);
  }

private:
  void * _object;
  Function _function;
};


template<typename T, typename Ret, typename... Args>
struct DelegateMaker
{
  using Method = Ret (T::*)(Args...);
  using Method_const = Ret (T::*)(Args...) const;

  template<Method method>
  static Ret evaluate(void* object, Args... args)
  {
    return (static_cast<T*>(object)->*method)(args...);
  }

  template<Method_const method>
  static Ret evaluate(void* object, Args... args)
  {
    return (static_cast<T*>(object)->*method)(args...);
  }

  template<Method method>
  inline static Delegate<Ret, Args...> bind(T* object)
  {
    return Delegate<Ret, Args...>(object, &DelegateMaker::evaluate<method>);
  }

  template<Method_const method>
  inline static Delegate<Ret, Args...> bind(T* object)
  {
    return Delegate<Ret, Args...>(object, &DelegateMaker::evaluate<method>);
  }
};


template<typename T, typename Ret, typename... Args>
DelegateMaker<T, Ret, Args...>
makeDelegate(Ret (T::*)(Args...))
{
  return DelegateMaker<T, Ret, Args...>();
}

template<typename T, typename Ret, typename... Args>
DelegateMaker<T, Ret, Args...>
makeDelegate(Ret (T::*)(Args...) const)
{
  return DelegateMaker<T, Ret, Args...>();
}


#define MakeDelegate(method, object) (makeDelegate(method).bind<method>(object))
#define MakeTemplateDelegate(method, object) (makeDelegate(method).template bind<method>(object))


int main()
{
  struct Test
  {
  public:
    int foo(int x) { return x*x; }
    int bar(int x) const { return x+x; }
  };

  Test test;

  auto foo = MakeDelegate(&Test::foo, &test);
  std::cout << foo(3) << std::endl;

  auto bar = MakeDelegate(&Test::bar, &test);
  std::cout << bar(3) << std::endl;
}
