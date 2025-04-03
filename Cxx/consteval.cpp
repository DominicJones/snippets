#include <type_traits>
#include <array>
#include <iostream>

template<typename A, typename B>
concept is_matching_size = requires(A a, B b) { a.size() == b.size(); };

template<typename V, typename I>
concept has_subscript_op = requires(V v, I i) { v[i]; };


consteval auto dot_product(has_subscript_op<int> auto a, has_subscript_op<int> auto b)
{
  static_assert(is_matching_size<decltype(a), decltype(b)>);
  using R = decltype(a[0] + b[0]);

  auto result = R(0);
  for (int it = 0; it != a.size(); ++it)
  {
    result += a[it] * b[it];
  }
  return result;
}


// IC = {3,4}
// DC = dual(IC,6) = {2,2,2,0,1,2}

template<auto size>
consteval auto dual(has_subscript_op<int> auto a)
{
  std::array<int, size> result;
  result.fill(a.size());

  int it_dual = 0;
  for (int it = 0; it != a.size(); ++it)
  {
    result[a[it]] = it_dual++;
  }
  return result;
}

consteval auto is_same(has_subscript_op<int> auto a, has_subscript_op<int> auto b)
{
  static_assert(is_matching_size<decltype(a), decltype(b)>);
  auto result = true;
  for (int it = 0; it != a.size(); ++it)
  {
    result = result && (a[it] == b[it]);
  }
  return result;
}


int main()
{
  std::array<int, 3> constexpr a{1,2,3};
  static_assert(dot_product(a, a) == (1*1 + 2*2 + 3*3));

  std::array<int, 2> constexpr b{3,4};
  static_assert(is_same(dual<6>(b), std::array<int, 6>{2,2,2,0,1,2}));
}
