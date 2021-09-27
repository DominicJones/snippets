/*
  Examples of using the NAG dco/map C++11 AD tool,
  base the following paper and other sources.

  http://sunsite.informatik.rwth-aachen.de/Publications/AIB/2017/2017-07.pdf
*/


// example 1

/*
 only one kind of terminal for input and output: the 'Active' type
 only const and non-const qualifiers distinguish input and output

 decltype(x1 * x2) = Binary<Mul, Active const &, Active const &>
 but   decltype(a) = Binary<Mul, Active const &, Active const &> const

 y is non-const and is not locally scoped
 dco_map::derivative(y) != 0
*/
template<class Active>
void foo(Active const &x1, Active const &x2, Active &y)
{
  const auto a = x1 * x2;
  const auto b = sin(a) * x1;
  y = b * exp(a);
}

int main()
{
  usign Active = dco_map::ga1s<double>::type;
  Active x1 = 3;
  Active x2 = 17;
  Active y1;
  dco_map::derivative(y1) = 1;
  foo(x1, x2, y1);
  printf("adjoint of x1 = %f \n", dco_map::derivative(x1));
  printf("adjoint of x2 = %f \n", dco_map::derivative(x2));
}


// example 2

template<class Active>
void bar(int n, Active const x[], Active &y)
{
  auto const a = sin(x[0]) * x[1];

  Active b = 0;
  MAP_FOR(Active, i, 2, n-1, 1) {
    const auto x2 = x[i] * x[i]; // what happens when x2 goes out of scope?
    b += sin((i + 1) * x2); // capture x2 by value?
  } MAP_FOR_END;

  Active c;
  MAP_CALL(Active, baz(a, b, c)); // in: [a,b], out: [c]

  Active d; // dco_map::derivative(d) == 0
  MAP_IF(Active, b < c) {
    d = exp(a) * b; // cache as opaque expr and eval in d's dtor?
  } MAP_ELSE {
    d = c * cos(b - a);
  } MAP_IF_END;
  
  y = a * b * c * d;
}
