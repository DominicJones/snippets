// dmd -O -release -run <file>

import std.numeric;
import std.stdio;
import std.datetime;

void main()
{
  immutable auto size = 100_000;
  auto a = new double[size];
  auto b = new double[size];

  a[] = 0.256;
  b[] = 0.512;

  StopWatch stopWatch;
  stopWatch.start();
  double c = dotProduct(a, b);
  stopWatch.stop();

  writefln("duration = %s", stopWatch.peek().nsecs);
  writefln("dot-product = %s", c);
}
