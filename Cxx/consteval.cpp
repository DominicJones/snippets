#include <type_traits>
#include <utility>
#include <array>


template<typename A, typename B>
concept is_equal_size = requires(A a, B b) { a.size() == b.size(); };

template<typename V, typename I = std::size_t>
concept has_subscript_op = requires(V v, I i) { v[i]; };


// generalise to inner_product with `op1(result, op2(a[it], b[it]))`
consteval auto dot_product(has_subscript_op<> auto a, has_subscript_op<> auto b)
{
  static_assert(is_equal_size<decltype(a), decltype(b)>);
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
consteval auto dual(has_subscript_op<> auto a)
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


// OS = {0,3}, L = {5,6,7,8,9};
// SL = subset(OS,L) = {5,8}
consteval auto subset(has_subscript_op<> auto os, has_subscript_op<> auto a)
{
  std::array<int, os.size()> result;
  
  for (int it = 0; it != os.size(); ++it)
  {
    result[it] = a[os[it]];
  }
  return result;
}


// L = {5,6,7};
// T = to_sequence(L) = sequence<5,6,7>
template<has_subscript_op<> auto A, typename S = std::make_index_sequence<A.size()> >
struct to_sequence;

template<auto A, auto... I>
struct to_sequence<A, std::index_sequence<I...> >
{
  using type = std::integer_sequence<int, A[I]...>;
};

template<auto A> using to_sequence_t = typename to_sequence<A>::type;


// S = sequence<5,6,7>
// L = to_array<S> = {5,6,7};
template<typename S> struct to_array;

template<auto... I>
struct to_array<std::integer_sequence<int, I...> >
{
  static auto constexpr value = std::array<int, sizeof...(I)>{I...};
};

template<typename S> static inline auto constexpr to_array_v = to_array<S>::value;


// elementwise identity
consteval auto is_same(has_subscript_op<> auto a, has_subscript_op<> auto b)
{
  static_assert(is_equal_size<decltype(a), decltype(b)>);
  auto result = true;
  for (int it = 0; it != a.size(); ++it)
  {
    result = result && (a[it] == b[it]);
  }
  return result;
}


// tests
int main()
{
  {
    std::array<int, 3> constexpr a{1,2,3};
    static_assert(dot_product(a, a) == (1*1 + 2*2 + 3*3));
  }
  {
    std::array<int, 2> constexpr a{3,4};
    static_assert(is_same(dual<6>(a), std::array<int, 6>{2,2,2,0,1,2}));
  }
  {
    std::array<int, 2> constexpr os{0,3};
    std::array<int, 5> constexpr a{5,6,7,8,9};
    static_assert(is_same(subset(os, a), std::array<int, 2>{5,8}));
  }
  {
    static std::array<int, 3> constexpr a{5,6,7};
    static_assert(std::is_same_v<to_sequence_t<a>, std::integer_sequence<int,5,6,7>>);
  }
  {
    using S = std::integer_sequence<int,5,6,7>;
    static_assert(is_same(to_array_v<S>, std::array<int, 3>{5,6,7}));
  }
}
