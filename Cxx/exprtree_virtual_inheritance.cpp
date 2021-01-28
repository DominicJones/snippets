struct Add
{
  static double primal(double a, double b) { return a + b; }
  static void adjoint(double a, double &ad, double b, double &bd, double rd)
  { ad += rd; bd += rd; }
};

struct Sub
{
  static double primal(double a, double b) { return a - b; }
  static void adjoint(double a, double &ad, double b, double &bd, double rd)
  { ad += rd; bd -= rd; }
};

struct Mul
{
  static double primal(double a, double b) { return a * b; }
  static void adjoint(double a, double &ad, double b, double &bd, double rd)
  { ad += b * rd; bd += a * rd; }
};

struct Div
{
  static double primal(double a, double b) { return a / b; }
  static void adjoint(double a, double &ad, double b, double &bd, double rd)
  { ad += rd / b; bd -= (a * rd) / (b * b); }
};




template<typename Tag>
struct Terminal
{
  Terminal(double const &pri)
  {
    this->pri = pri;
    this->adj = 0;
  }

  Terminal() {}

  void primal() const {}

  void adjoint() const {}

  static double pri;
  static double adj;
};

template<typename Tag>
double Terminal<Tag>::pri = 0;

template<typename Tag>
double Terminal<Tag>::adj = 0;




template<typename OP, typename E0, typename E1>
struct Binary: virtual E0, virtual E1
{
  Binary(E0 const &e0, E1 const &e1)
    : E0(e0), E1(e1)
  {}

  Binary() {}

  void primal() const
  {
    E0 const &e0(static_cast<E0 const &>(*this));
    E1 const &e1(static_cast<E1 const &>(*this));
    e0.primal();
    e1.primal();
    this->pri = OP::primal(e0.pri, e1.pri);
  }

  void adjoint() const
  {
    E0 const &e0(static_cast<E0 const &>(*this));
    E1 const &e1(static_cast<E1 const &>(*this));
    OP::adjoint(e0.pri, e0.adj, e1.pri, e1.adj, this->adj);
  }

  static double pri;
  static double adj;
};

template<typename OP, typename E0, typename E1>
double Binary<OP, E0, E1>::pri = 0;

template<typename OP, typename E0, typename E1>
double Binary<OP, E0, E1>::adj = 0;




template<typename OP, typename E0>
struct Binary<OP, E0, double>: virtual E0
{
  Binary(E0 const &e0, double const &e1_pri)
    : E0(e0)
  {
    this->e1_pri = e1_pri;
  }

  Binary() {}

  void primal() const
  {
    E0 const &e0(static_cast<E0 const &>(*this));
    e0.primal();
    this->pri = OP::primal(e0.pri, e1_pri);
  }

  void adjoint() const
  {
    E0 const &e0(static_cast<E0 const &>(*this));
    double e1_adj{0};
    OP::adjoint(e0.pri, e0.adj, e1_pri, e1_adj, this->adj);
  }

  static double e1_pri;
  static double pri;
  static double adj;
};

template<typename OP, typename E0>
double Binary<OP, E0, double>::e1_pri = 0;

template<typename OP, typename E0>
double Binary<OP, E0, double>::pri = 0;

template<typename OP, typename E0>
double Binary<OP, E0, double>::adj = 0;




template<typename T> struct PrimalEvaluator {};

template<typename OP, typename E0, typename E1>
struct PrimalEvaluator<Binary<OP, E0, E1> >
  : virtual Binary<OP, PrimalEvaluator<E0>, PrimalEvaluator<E1> >
{
  PrimalEvaluator()
  {
    reinterpret_cast<Binary<OP, E0, E1> const *>(this)->primal();
  }
};

template<typename OP, typename E0>
struct PrimalEvaluator<Binary<OP, E0, double> >
  : virtual Binary<OP, PrimalEvaluator<E0>, double>
{
  PrimalEvaluator()
  {
    reinterpret_cast<Binary<OP, E0, double> const *>(this)->primal();
  }
};



template<typename T> struct AdjointEvaluator {};

template<typename OP, typename E0, typename E1>
struct AdjointEvaluator<Binary<OP, E0, E1> >
  : virtual Binary<OP, AdjointEvaluator<E0>, AdjointEvaluator<E1> >
{
  ~AdjointEvaluator()
  {
    reinterpret_cast<Binary<OP, E0, E1> const *>(this)->adjoint();
  }
};

template<typename OP, typename E0>
struct AdjointEvaluator<Binary<OP, E0, double> >
  : virtual Binary<OP, AdjointEvaluator<E0>, double>
{
  ~AdjointEvaluator()
  {
    reinterpret_cast<Binary<OP, E0, double> const *>(this)->adjoint();
  }
};




#include <iostream>

struct A;
struct B;

int main()
{
  double a_pri = 3;
  double b_pri = 4;
  double w_pri;

  double a_adj = 0;
  double b_adj = 0;
  double w_adj = 1;

  {
    Terminal<A> a{double(a_pri)};
    Terminal<B> b{double(b_pri)};

    double c0 = 6;
    double c1 = 7;

    Binary<Mul, decltype(a), decltype(b)> t{a, b};
    Binary<Add, decltype(t), double>      u{t, c0};
    Binary<Sub, decltype(t), double>      v{t, c1}; // `Add' causes compilation error: duplicate base type <X> invalid
    Binary<Div, decltype(u), decltype(v)> w{u, v};

    // primal
    PrimalEvaluator<decltype(w)>{};
    w_pri = w.pri;

    // adjoint
    w.adj = w_adj;
    AdjointEvaluator<decltype(w)>{};
    a_adj += a.adj;
    b_adj += b.adj;
  }

  std::cout << "a     = " << a_pri << std::endl;
  std::cout << "b     = " << b_pri << std::endl;
  std::cout << "w     = " << w_pri << std::endl;
  std::cout << "a-adj = " << a_adj << std::endl;
  std::cout << "b-adj = " << b_adj << std::endl;
}
