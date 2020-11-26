template<class Active>
void foo(Active const &x1, Active const &x2, Active &y)
{
  const auto a = x1 * x2;
  const auto b = sin(a) * x1;
  y = b * exp(a);
}

{
  usign Active = dco_map::ga1s<double>::type;
  Active x1 = 3, x2 = 17;
  Active y1;
  dco_map::derivative(y1) = 1;
  foo(x1 , x2 , y1);
}


template<class Active>
void foo(int n, Active const x[], Active &y)
{
  auto const a = sin(x[0]) * x[1];
  Active a, b(0), c, d;

  MAP_FOR(Active, i, 2, n-1, 1)
  {
    b += x[i];
  } MAP_FOR_END;

  MAP_CALL(Active, bar(a, b, c));

  MAP_IF(Active , b < c) {
    d = exp(a) * b;
  } MAP_ELSE {
    d = c * cos(b - a);
  } MAP_IF_END ;

  y = d * c * b * a;
}
