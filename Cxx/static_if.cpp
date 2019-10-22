// static-if in c++14


#include <utility>


#define cpp_unused(x) ((void)(x))


namespace cpp {
namespace static_if_detail {

struct identity
{
  template <typename T>
  T operator()(T&& x) const
  {
    return std::forward<T>(x);
  }
};


template <bool Cond>
struct statement
{
  template <typename F>
  void then(const F& f)
  {
    f(identity());
  }

  template <typename F>
  void else_(const F& f)
  {
    cpp_unused(f);
  }
};


template <>
struct statement<false>
{
  template <typename F>
  void then(const F& f)
  {
    cpp_unused(f);
  }

  template <typename F>
  void else_(const F& f)
  {
    f(identity());
  }
};

}


template <bool Cond, typename F>
static_if_detail::statement<Cond> static_if(F const& f)
{
  static_if_detail::statement<Cond> if_;
  if_.then(f);
  return if_;
}

}


//


#include <iostream>

template <bool primal>
struct Execute
{
  static auto execute(auto &x)
  {
    cpp::static_if<primal>([&](auto y)
    {
      y(x) = 111;
      std::cout << "first branch" << std::endl;
    }).else_([&](auto y)
    {
      y(x) = 222;
      std::cout << "second branch" << std::endl;
    });
  }
};


auto f(auto &x)
{

  Execute<false>::execute(x);
  return Execute<true>::execute(x);
}


int main()
{
  int x = 2;
  f(x);
  std::cout << x << std::endl;
}
