#include <utility>

struct Term {};

namespace regular
{
  struct Wrapper
  {
    Wrapper(Term v) {}
  };

  Wrapper evaluate(Term &&v)
  {
    return Wrapper(std::forward<Term>(v));
  }
}

namespace templated
{
  template<class T>
  struct Wrapper
  {
    Wrapper(T v) {}
  };

  template<class T>
  Wrapper<T> evaluate(T &&v)
  {
    return Wrapper<T>(std::forward<T>(v));
  }
}

int main()
{
  {
    using namespace regular;
    Term const v;

    // error: cannot bind rvalue reference of type ‘Term&&’ to lvalue of type ‘const Term’
    auto w = evaluate(v);
  }

  {
    using namespace templated;
    Term const v;
    auto w = evaluate(v);
  }
}
