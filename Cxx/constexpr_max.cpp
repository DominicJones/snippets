template<class T>
constexpr T constexpr_max(T x)
{
  return T(x);
}

template<class T, class... Ts>
constexpr T constexpr_max(T x, Ts... xs)
{
  T m = constexpr_max(xs...);
  return T(x > m? x : m);
}
