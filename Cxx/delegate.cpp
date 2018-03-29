#include <iostream>
#include <type_traits>

struct Primal {};
struct Tangent {};
struct Adjoint {};


template<class Object_t, class Method_t, class Mode_t> class DrvScope
{
public:
  DrvScope(Object_t const &object, Method_t const &method)
    : _object(object), _method(method)
  {
    if (std::is_same<Mode_t, Tangent>::value)
      {
        (_object.*_method)();
      }
  }

  ~DrvScope()
  {
    if (std::is_same<Mode_t, Adjoint>::value)
      {
        (_object.*_method)();
      }
  }

private:
  Object_t const &_object;
  Method_t const _method;
};


class Evaluate
{
public:
  void solve() const
  {
    std::cout << "Evaluate::solve" << std::endl;
  }
};


int main()
{
  Evaluate evaluate;
  DrvScope<Evaluate, void(Evaluate::*)() const, Primal> sf1(evaluate, &Evaluate::solve);
  DrvScope<Evaluate, void(Evaluate::*)() const, Tangent> sf2(evaluate, &Evaluate::solve);
  DrvScope<Evaluate, void(Evaluate::*)() const, Adjoint> sf3(evaluate, &Evaluate::solve);
}
