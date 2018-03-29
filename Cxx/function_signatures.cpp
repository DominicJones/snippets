#include <type_traits>


struct Mode {};
struct Primal : Mode {};
struct Tangent : Mode {};
struct Adjoint : Mode {};


template<class Mode_t>
typename std::enable_if<std::is_same<Primal, Mode_t>::value, double>::type
eval(double const &x)
{
  double const f = x * x;
  return f;
}


template<class Mode_t>
typename std::enable_if<std::is_same<Tangent, Mode_t>::value, double>::type
eval(double const &x, double const &x_drv, double &f_drv)
{
  double const f = eval<Primal>(x);
  f_drv = 2 * x * x_drv;
  return f;
}


template<class Mode_t>
typename std::enable_if<std::is_same<Adjoint, Mode_t>::value, double>::type
eval(double const &x, double &x_drv, double const &f_drv)
{
  double const f = eval<Primal>(x);
  x_drv += 2 * x * f_drv;
  return f;
}


int main()
{
  double const x = 2;
  double x_adj = 0;

  double f;
  double f_tng;

  f = eval<Primal>(x);
  f = eval<Tangent>(x, double(1), f_tng);
  f = eval<Adjoint>(x, x_adj, double(1));
}
