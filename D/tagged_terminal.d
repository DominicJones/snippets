// at: https://run.dlang.io/is/kdw0CE

struct Terminal(T, string file = __FILE__, size_t line = __LINE__)
{
  T value;

  auto opBinary(string op, R)(const ref R r)
  {
     return Binary!(op, typeof(this), R)(this, r);
  }
}

struct Binary(string op, L, R)
{
  L l;
  R r;
}

void main()
{
  Terminal!double c0;
  Terminal!double c1;
  pragma(msg, typeof(c0 + c1));
}
