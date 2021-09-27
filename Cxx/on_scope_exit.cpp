// https://codereview.stackexchange.com/questions/134234/on-the-fly-destructors

#include <iostream>
#include <utility>
#include <type_traits>


#define JOIN(symbol1, symbol2) JOIN_IMPL(symbol1, symbol2)
#define JOIN_IMPL(symbol1, symbol2) symbol1 ## symbol2


template<typename F, typename... Args>
auto makeLambda(F &&f, Args &&... f_args) noexcept
{
  return [=]() -> std::result_of_t<F(Args...)>
  {
    return f(std::forward<Args>(f_args)...);
  };
}


template<typename F>
auto onScopeExit(F &&f) noexcept
{
  struct OnScopeExit
  {
    using fn_t = decltype(makeLambda(std::move(f)));
    fn_t fn;

    OnScopeExit(F &&fn)
      : fn{makeLambda(std::move(fn))}
    {}

    ~OnScopeExit() { fn(); }
  };

  return OnScopeExit{std::move(f)};
}


int main()
{
  int x = 3;
  auto JOIN(lambda, __LINE__) = onScopeExit([&]{ std::cout << x << std::endl; x--; });
  auto JOIN(lambda, __LINE__) = onScopeExit([&]{ std::cout << x << std::endl; x--; });
  auto JOIN(lambda, __LINE__) = onScopeExit([&]{ std::cout << x << std::endl; x--; });
}
