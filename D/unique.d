struct Unique(T, size_t line)
{
  T value;
}

auto UQ(T, size_t line = __LINE__)(auto ref T value)
{
  return Unique!(T, line)(value);
}

void main()
{
  auto c0 = UQ(3);
  auto c1 = UQ(4);
  static assert(!is(typeof(c0) == typeof(c1)));
}
