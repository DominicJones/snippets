// at: https://godbolt.org/z/CLPzGq

template<typename T, typename U>
auto constexpr cmp(T const &t, U const &u)
{
/* Preferable, but not possible:
     auto constexpr ta = &t;
     auto constexpr ua = &u;
     return ta == ua;
*/
  return &t == &u;
}

int main()
{
  auto c0 = 1;
  auto c1 = 2;

  static_assert(cmp(c0, c0));
  static_assert(!cmp(c0, c1));
}
