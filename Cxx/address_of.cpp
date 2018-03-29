
template<typename T, typename U>
auto constexpr cmp(T const &t, U const &u)
{
  return &t == &u;
}


int main()
{
  int x = 3;
  int y = 4;

  static_assert(cmp(x, x));
  static_assert(!cmp(x, y));
}
