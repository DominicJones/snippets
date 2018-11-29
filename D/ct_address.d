// at: https://run.dlang.io/is/rf5azp

auto cmp(T, U)(const ref T t, const ref U u)
{
  return &t == &u;
}

void main()
{
  auto c0 = 1;
  auto c1 = 2;

  // error: "variable c0 cannot be read at compile time"
  static assert(cmp(c0, c0));
  static assert(!cmp(c0, c1));
}
