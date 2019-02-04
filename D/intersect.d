import std.stdio, std.format, std.math,
  std.numeric, std.traits, std.array,
  std.conv, std.container, std.algorithm;


/**
  Are two straight lines intersecting?
*/
void main(string[] args)
{
  auto p = Vector!()(0, 0);
  auto q = Vector!()(0.5001, 0.50001);
  auto l_pq = Line!()(p, q);

  auto r = Vector!()(1, 0);
  auto s = Vector!()(0, 1);
  auto l_rs = Line!()(r, s);

  auto t_len = l_pq.lengthsAtIntersection(l_rs);

  if (l_pq.intersect(l_rs, t_len))
    writefln("The lines intersect at %s", l_pq.point(t_len[0]).v[]);
  else
    writeln("The lines do not intersect.");
}


unittest
{
  auto p = Vector!()(0, 0);
  auto q = Vector!()(1, 1);
  auto l_pq = Line!()(p, q);

  auto r = Vector!()(1, 0);
  auto s = Vector!()(0, 1);
  auto l_rs = Line!()(r, s);

  auto t_len = l_pq.lengthsAtIntersection(l_rs);
  assert (l_pq.intersect(l_rs, t_len));
}


struct Vector(int N = 3, T = float)
if (isFloatingPoint!T)
{
  T[N] v;

  this (T...)(T v)
  {
    this.v[] = 0;
    foreach(i, e; v)
      this.v[i] = e;
  }

  auto dot(ref Vector!(N, T) op2)
  {
    T reduce = 0;
    for (int i = 0; i != this.v.length; ++i)
      reduce += this.v[i] * op2.v[i];
    return reduce;
  }

  auto mag()
  {
    return sqrt(this.dot(this));
  }

  auto unit()
  {
    typeof(v) map = this.v[] / this.mag();
    return map;
  }
}


struct Line(int N = 3, T = float)
if (isFloatingPoint!T)
{
  Vector!(N, T) x0, x1;
  this (ref Vector!(N, T) x0, ref Vector!(N, T) x1)
  {
    this.x0.v[] = x0.v[];
    this.x1.v[] = x1.v[];
  }

  auto length()
  {
    Vector!(N, T) dx;
    dx.v[] = x1.v[] - x0.v[];
    return dx.mag();
  }

  auto unit()
  {
    typeof(x0) dx;
    dx.v[] = this.x1.v[] - this.x0.v[];
    return dx.unit();
  }

  T[2] lengthsAtIntersection(ref Line!(N, T) l1)
  {
    T[2] b;
    b[] = l1.x0.v[0..2] - this.x0.v[0..2];

    T[2][2] A;
    A[0][] = this.unit()[0..2];
    A[1][] = -l1.unit()[0..2];
    transpose!(2, T)(A);
    bool stat = invert!(2, T)(A);
    if (!stat)
      {
        b[] = -1;
        return b;
      }

    multiply!(2, T)(A, b);
    return b;
  }

  bool intersect(ref Line!(N, T) l1, ref T[2] len)
  {
    if (len[0] > 0 && len[0] < l1.length() &&
        len[1] > 0 && len[1] < this.length())
      {
        return true;
      }
    else
      {
        return false;
      }
  }

  auto point(T len)
  {
    typeof(x0) x;
    x.v[] = this.x0.v[] + len * this.unit()[];
    return x;
  }
}


void transpose(int N, T)(ref T[N][N] mat)
{
  for (int k = 0; k != N*N; ++k)
    {
      int i = k/N, j = k%N;
      swap(mat[i][j], mat[j][i]);
    }
}


void multiply(int N, T)(ref T[N][N] mat, ref T[N] rhs)
{
  T[N] res;
  for (int i = 0; i != N; ++i)
    {
      T reduce = 0;
      for (int j = 0; j != N; ++j)
        {
          reduce += mat[i][j] * rhs[j];
        }
      res[i] = reduce;
    }
  rhs[] = res[];
}


bool invert(int N, T)(ref T[N][N] mat)
  if ((N == 2 || N == 3) && isFloatingPoint!T)
{
  T[N][N] inv;

  static if (N == 2)
    {
      T det = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
    }
  else
    {
      T det
        = (mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1]) * mat[0][0]
        + (mat[1][2]*mat[2][0] - mat[1][0]*mat[2][2]) * mat[0][1]
        + (mat[1][0]*mat[2][1] - mat[1][1]*mat[2][0]) * mat[0][2];
    }

  if (abs(det) < 1.0e-10)
    return false;

  static if (N == 2)
    {
      inv[0][0]= mat[1][1];
      inv[0][1]=-mat[0][1];
      inv[1][0]=-mat[1][0];
      inv[1][1]= mat[0][0];
    }
  else
    {
      inv[0][0]=mat[1][1]*mat[2][2]-mat[1][2]*mat[2][1];
      inv[1][0]=mat[1][2]*mat[2][0]-mat[1][0]*mat[2][2];
      inv[2][0]=mat[1][0]*mat[2][1]-mat[1][1]*mat[2][0];
      inv[0][1]=mat[2][1]*mat[0][2]-mat[2][2]*mat[0][1];
      inv[1][1]=mat[2][2]*mat[0][0]-mat[2][0]*mat[0][2];
      inv[2][1]=mat[2][0]*mat[0][1]-mat[2][1]*mat[0][0];
      inv[0][2]=mat[0][1]*mat[1][2]-mat[0][2]*mat[1][1];
      inv[1][2]=mat[0][2]*mat[1][0]-mat[0][0]*mat[1][2];
      inv[2][2]=mat[0][0]*mat[1][1]-mat[0][1]*mat[1][0];
    }

  for (int k = 0; k != N*N; ++k)
    {
      int i = k/N, j = k%N;
      mat[i][j] = inv[i][j] / det;
    }

  return true;
}
