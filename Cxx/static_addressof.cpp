#include <type_traits>
#include <utility>
#include <array>
#include <iostream>


// The ‘address of’ operator can be used in a constant expression context,
// but only for comparison. If it were possible to leverage this feature of the language
// in the desired context it would have been minimally useful.
// Furthermore, if, in addition, it were possible to capture the values being compared
// it would be sufficiently useful. However, neither are possible.
auto consteval is_same_var_fn(auto const &t, auto const &u)
{
//  auto constexpr ta = &t;
//  auto constexpr ua = &u;
//  return ta == ua;
  return &t == &u;
}

template<auto const &t, auto const &u> struct is_same_var
{
  static auto constexpr value0 = &t;
  static auto constexpr value1 = &u;
  static auto constexpr value = (&t == &u);
};


// tests
int main()
{
  {
    static auto constexpr c0 = 7;
    static auto constexpr c1 = 9;
    
    static_assert(is_same_var_fn(c0, c0));
    static_assert(!is_same_var_fn(c0, c1));

    static_assert(is_same_var<c0, c0>::value);
    static_assert(!is_same_var<c0, c1>::value);

    std::cout<<is_same_var<c0, c1>::value0<<std::endl;
    std::cout<<is_same_var<c0, c1>::value1<<std::endl;
  }
}
