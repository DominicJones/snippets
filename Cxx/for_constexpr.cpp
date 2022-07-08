// C++17, except C++20 for version_1 (but could be rewritten in C++17)


// a simple example of evaluation of a regular function at compile time
int constexpr pow(int base, int exp) noexcept
{
  // if constexpr (exp == 0) { return 1; } else { return base * pow(base, exp - 1); } // error
  return (exp == 0)? (1): (base * pow(base, exp - 1)); // ok
}

static_assert(pow(3, 2) == 9);
static_assert(pow(4, 2) == 16);
static_assert(pow(5, 2) == 25);


// c-t factorial using template syntax
// template args are required when using 'if constexpr'
namespace version_1
{
  template<int begin, int end, int incr, typename Fn>
  constexpr void for_constexpr(Fn &&fn)
  {
    if constexpr (begin != end)
    {
      fn.template operator()<begin>();
      for_constexpr<(begin + incr), end, incr>(fn);
    }
  }

  template<int end>
  constexpr auto factorial()
  {
    auto result = 1;
    for_constexpr<0, end, 1>([&result]<auto it>(){ result *= (it+1); });
    return result;
  }

  static_assert(factorial<3>() == 6);
  static_assert(factorial<4>() == 24);
  static_assert(factorial<5>() == 120);
}


// c-t factorial using function syntax
// ternary operator can work with regular args at compile time
namespace version_2
{
  template<typename Fn>
  void constexpr for_constexpr(int begin, int end, int incr, Fn &&fn) noexcept
  {
    (begin != end)? (fn(begin), for_constexpr((begin + incr), end, incr, fn), void()) : void();
  }

  auto constexpr factorial(int end) noexcept
  {
    auto result = 1;
    for_constexpr(0, end, 1, [&result](int it){ result *= (it+1); });
    return result;
  }

  static_assert(factorial(3) == 6);
  static_assert(factorial(4) == 24);
  static_assert(factorial(5) == 120);
}


int main() {}
