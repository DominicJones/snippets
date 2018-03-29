// dmd -unittest -main -run <file>

import std.stdio;

unittest
{
  int[2] x;
  int[2] y;

  x[] = 2;

  eval(x, y);
  writeln(x, y);
}

unittest
{
  auto x = new int[2];
  auto y = new int[2];

  x[] = 2;

  eval(x, y);
  writeln(x, y);
}

void eval(in int[] x, int[] y)
{
  y[] = x[]^^2;
}
