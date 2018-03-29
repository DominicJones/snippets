import std.stdio;

void print(alias pred)(int x, int y)
{
  writeln(pred(x, y));
}

void print_r1(alias pred, T)(T x, T y)
{
  writeln(pred(x, y));
}

void print_r2(alias pred, T...)(T args)
{
  writeln(pred(args));
}

void main()
{
  int x = 2, y = 3;
  print!((a, b) => a * b)(x, y);
  print_r1!((a, b) => a * b)(x, y);

  double z = 1;
  print_r2!(() => 6)();
  print_r2!((a, b) => a * b)(x, y);
  print_r2!((a, b, c) => a * b * c)(x, y, z);
}
