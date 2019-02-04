import std.stdio, std.conv, std.math;

/**
   solve the equation
   a.x^^2 + b.x + c = 0
   using the algorithm
   x1,x2  = (-b +- sqrt(b^^2 - 4ac)) / 2a
*/
void main(string[] args)
{
  float a, b, c;

  if (args.length < 4)
    {
      writeln("Enter values for a, b and c");
      readf("%s", &a);
      readf("%s", &b);
      readf("%s", &c);
      writeln("a,b,c ", [a, b, c]);
    }
  else
    {
      a = to!float(args[1]);
      b = to!float(args[2]);
      c = to!float(args[3]);
    }

  float x[2];
  float sign[] = [-1, 1];
  float disc = pow(b, 2) - 4 * a * c;

  if (disc <= 0)
    {
      writeln("no real solutions");
    }
  else
    {
      float sqrt_disc;
      sqrt_disc = sqrt(disc);
      x[] = (-b + sign[] * sqrt_disc) / 2 * a;
      writeln("x1, x2 ", x);
    }
}
