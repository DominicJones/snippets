// g++ -std=c++11 exprtree.cpp && ./a.out

#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <array>
#include <tuple>
#include <utility>
#include <cstring>


struct Pos
{
  static char const * name() { return "+"; }
  static double primal(double a) { return +a; }
  static double tangent(double a, double ad) { return +ad; }
  static void adjoint(double a, double &ad, double rd) { ad += rd; }
};

struct Neg
{
  static char const * name() { return "-"; }
  static double primal(double a) { return -a; }
  static double tangent(double a, double ad) { return -ad; }
  static void adjoint(double a, double &ad, double rd) { ad -= rd; }
};

struct Add
{
  static char const * name() { return "+"; }
  static double primal(double a, double b) { return a + b; }
  static double tangent(double a, double ad, double b, double bd) { return ad + bd; }
  static void adjoint(double a, double &ad, double b, double &bd, double rd) { ad += rd; bd += rd; }
};

struct Sub
{
  static char const * name() { return "-"; }
  static double primal(double a, double b) { return a - b; }
  static double tangent(double a, double ad, double b, double bd) { return ad - bd; }
  static void adjoint(double a, double &ad, double b, double &bd, double rd) { ad += rd; bd -= rd; }
};

struct Mul
{
  static char const * name() { return "*"; }
  static double primal(double a, double b) { return a * b; }
  static double tangent(double a, double ad, double b, double bd) { return ad * b + bd * a; }
  static void adjoint(double a, double &ad, double b, double &bd, double rd) { ad += b * rd; bd += a * rd; }
};

struct Div
{
  static char const * name() { return "/"; }
  static double primal(double a, double b) { return a / b; }
  static double tangent(double a, double ad, double b, double bd) { return (ad * b - a * bd) / (b * b); }
  static void adjoint(double a, double &ad, double b, double &bd, double rd) { ad += rd / b; bd -= a * rd / (b * b); }
};


template<typename T>
struct Terminal
{
  using type = Terminal const &;
  double eval_primal() const { return value; }
  double eval_tangent() const { return deriv; }
  void eval_adjoint(double deriv) const { this->deriv += deriv; }
  mutable double value;
  mutable double deriv;
};


template<typename F, typename L>
struct Unary
{
  using type = Unary const;

  Unary(L const &l) : l(l) {}

  double eval_primal() const
  {
    double l_value{l.eval_primal()};
    return F::primal(l_value);
  }

  double eval_tangent() const
  {
    double l_value{l.eval_primal()};
    double l_deriv{l.eval_tangent()};
    return F::tangent(l_value, l_deriv);
  }

  void eval_adjoint(double deriv) const
  {
    double l_value{l.eval_primal()};
    double l_deriv{0};
    F::adjoint(l_value, l_deriv, deriv);
    l.eval_adjoint(l_deriv);
  }

  typename L::type l;
};


template<typename F, typename L, typename R>
struct Binary
{
  using type = Binary const;

  Binary(L const &l, R const &r) : l(l), r(r) {}

  double eval_primal() const
  {
    double l_value{l.eval_primal()};
    double r_value{r.eval_primal()};
    return F::primal(l_value, r_value);
  }

  double eval_tangent() const
  {
    double l_value{l.eval_primal()};
    double r_value{r.eval_primal()};
    double l_deriv{l.eval_tangent()};
    double r_deriv{r.eval_tangent()};
    return F::tangent(l_value, l_deriv, r_value, r_deriv);
  }

  void eval_adjoint(double deriv) const
  {
    double l_value{l.eval_primal()};
    double r_value{r.eval_primal()};
    double l_deriv{0};
    double r_deriv{0};
    F::adjoint(l_value, l_deriv, r_value, r_deriv, deriv);
    l.eval_adjoint(l_deriv);
    r.eval_adjoint(r_deriv);
  }

  typename L::type l;
  typename R::type r;
};


template<typename T>
std::ostream &operator<<(std::ostream &os, Terminal<T> const &expr)
{
  os << std::string{T::name()};
  return os;
}

template<typename F, typename L>
std::ostream &operator<<(std::ostream &os, Unary<F, L> const &expr)
{
  os << "(" << std::string{F::name()} << expr.l << ")";
  return os;
}

template<typename F, typename L, typename R>
std::ostream &operator<<(std::ostream &os, Binary<F, L, R> const &expr)
{
  os << "(" << std::string{F::name()} << expr.l << "," << expr.r << ")";
  return os;
}


#define GenerateUnaryOperator(Fn, Op)                   \
  template<typename L>                                  \
  constexpr Unary<Fn, L>                                \
  Op(L const &l)                                        \
  {                                                     \
    return {l};                                         \
  }
GenerateUnaryOperator(Pos, operator+);
GenerateUnaryOperator(Neg, operator-);
#undef GenerateUnaryOperator


#define GenerateBinaryOperator(Fn, Op)                                  \
  template<typename L, typename R>                                      \
  constexpr Binary<Fn, L, R>                                            \
  Op(L const &l, R const &r)                                            \
  {                                                                     \
    return {l, r};                                                      \
  }
GenerateBinaryOperator(Add, operator+);
GenerateBinaryOperator(Sub, operator-);
GenerateBinaryOperator(Mul, operator*);
GenerateBinaryOperator(Div, operator/);
#undef GenerateBinaryOperator


struct A { static char const * name() { return "A"; } };
struct B { static char const * name() { return "B"; } };
struct C { static char const * name() { return "C"; } };


int main()
{
  Terminal<A> a{3};
  Terminal<B> b{4};
  Terminal<C> c{5};

  auto r = -a * b / c;

  std::cout << std::endl;
  std::cout << "PRIMAL" << std::endl;
  std::cout << a << " = " << a.value << std::endl;
  std::cout << b << " = " << b.value << std::endl;
  std::cout << c << " = " << c.value << std::endl;
  std::cout << r << " = " << r.eval_primal() << std::endl;

  std::cout << std::endl;
  std::cout << "TANGENT" << std::endl;
  a.deriv = 1; b.deriv = 0; c.deriv = 0;
  std::cout << a << " = " << a.deriv << std::endl;
  std::cout << b << " = " << b.deriv << std::endl;
  std::cout << c << " = " << c.deriv << std::endl;
  std::cout << r << " = " << r.eval_tangent() << std::endl;

  std::cout << std::endl;
  std::cout << "ADJOINT" << std::endl;
  a.deriv = 0; b.deriv = 0; c.deriv = 0;
  r.eval_adjoint(1);
  std::cout << a << " = " << a.deriv << std::endl;
  std::cout << b << " = " << b.deriv << std::endl;
  std::cout << c << " = " << c.deriv << std::endl;
}
