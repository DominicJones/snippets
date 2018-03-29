// From:
//   Meta Adjoint Programming in C++
//   Department of Computer Science Technical Report
//   RWTH Aachen University


using type = dco_map::ga1s<double>::type;


// example 1: basics
{
  type x(2.0), y;
  dco_map::derivative(y) = 1.0;

  {
    const auto t = x*x;
    y = sin(t);
  }

  printf("adjoint of x = %f\n", dco_map::derivative(x));
}
/*
  observations:
  1. t is an expression type (as detailed in the paper)
  2. nested scope implies use of non-trivial destructors
  3. adjoint evaluation takes place on assignment to y, or destruction of y, or on request.
     But y destructs after printf so not upon destruction, also derivative(x) does not know
     about the expression tree so not on request. Therefore, evaluation is upon assignment.
 */


// example 2: if-else
{
  type x(2.0), y;
  dco_map::derivative(y) = 1.0;

  {
    type t;

    MAP_IF(type, x <= 2.0)
    {
      t = x*x;
    }
    MAP_ELSE
    {
      t = sin(x);
    }
    MAP_IF_END;

    y = sin(t);
  }

  printf("adjoint of x = %f\n", dco_map::derivative(x));
}
/*
  observations:
  1. t does not know about the conditional expression
  2. t only knows about the assigned expression opaquely (i.e. in a typeless way)
  3. if evaluation takes place on assignment, it would be premature for t.
     Somehow MAP_IF prevents t being evaluated upon assignment, and instead
     delays it to destruction.
 */


// example 3: for loop
{
  int n = 2;
  type x(2.0), y;
  dco_map::derivative(y) = 1.0;

  {
    type sum = 0.0;

    MAP_FOR(type, i, 0, n-1, 1)
    {
      const auto t = x*x;
      sum += sin((i+1)*t);
    }
    MAP_FOR_END;

    y = sin(sum);
  }

  printf("adjoint of x = %f\n", dco_map::derivative(x));
}


// example 4: function call
{
  template<typename T> void f(const T& x, T& y) { y = x*x; }

  type x(2.0), y;
  dco_map::derivative(y) = 1.0;

  {
    type t;
    MAP_CALL(type, f(x, t));
    y = sin(t);
  }

  printf("adjoint of x = %f\n", dco_map::derivative(x));
}
