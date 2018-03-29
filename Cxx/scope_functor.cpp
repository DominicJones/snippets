#include <iostream>


struct Scope { enum Option { entry, exit, }; };


class Evaluate
{
public:
  void solve() const
  {
    std::cout << "* Evaluate::solve *" << std::endl;
  }
};


template<class Host_t> class ScopeFunctor
{
private:
  typedef void (Host_t::*Method_t)() const;

public:
  ScopeFunctor(Host_t const &host,
               Method_t const &method,
               Scope::Option const &scopeOption)
    : _host(host)
    , _method(method)
    , _scopeOption(scopeOption)
  {
    if (_scopeOption == Scope::entry)
      ((_host).*(_method))();
  }

  void operator()()
  {
    std::cout << "* ScopeFunctor::operator() * " << std::endl;
  }

  ~ScopeFunctor()
  {
    if (_scopeOption == Scope::exit)
      ((_host).*(_method))();
  }

private:
  Host_t const &_host;
  Method_t const &_method;
  Scope::Option const _scopeOption;
};


int main()
{
  Evaluate evaluate;

  {
    ScopeFunctor<Evaluate> sf(evaluate, &Evaluate::solve, Scope::exit);
    sf();
  }

  {
    ScopeFunctor<Evaluate> sf(evaluate, &Evaluate::solve, Scope::entry);
    sf();
  }
}
