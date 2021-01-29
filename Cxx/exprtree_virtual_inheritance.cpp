#include <type_traits>
#include <typeinfo>
#include <iostream>

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
struct Const
{
  Const(double const &value) : value(value) {}
  double value;
};


template<typename Tag>
struct Terminal
{
  Terminal(double const &pri, double &adj)
  {
    this->pri = pri;
    this->adj = adj;
    this->adjptr = &adj;
  }

  Terminal() {}
  void primal() const {}
  void adjoint() const { *(this->adjptr) += adj; }

  static double pri;
  static double adj;
  static double * adjptr;
};

template<typename Tag>
double Terminal<Tag>::pri = 0;

template<typename Tag>
double Terminal<Tag>::adj = 0;

template<typename Tag>
double * Terminal<Tag>::adjptr = nullptr;




template<typename A, typename B,
         bool isUnique = !std::is_same<A,B>::value>
struct UniqueBase
  : virtual A, virtual B
{
  UniqueBase() {}
  UniqueBase(const A& a, const B& b) : A(a), B(b) {}
};

template<typename A, typename B>
struct UniqueBase<A, B, false>
  : virtual A
{
  UniqueBase() {}
  UniqueBase(const A& a, const B& b) : A(a) {}
};




template<typename OP, typename E0, typename E1>
struct Binary
  : virtual UniqueBase<E0, E1>
{
  Binary(E0 const &e0, E1 const &e1)
    : UniqueBase<E0, E1>(e0,e1)
  {}

  Binary() {}

  void primal() const
  {
    E0 const &e0(static_cast<E0 const &>(*this));
    E1 const &e1(static_cast<E1 const &>(*this));
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


template<typename OP, typename E0, typename E1Tag>
struct Binary<OP, E0, Const<E1Tag> >
  : virtual E0
{
  Binary(E0 const &e0, Const<E1Tag> const &e1_pri)
    : E0(e0)
  {
    this->e1_pri = e1_pri.value;
  }

  Binary() {}

  void primal() const
  {
    E0 const &e0(static_cast<E0 const &>(*this));
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

template<typename OP, typename E0, typename E1Tag>
double Binary<OP, E0, Const<E1Tag> >::e1_pri = 0;

template<typename OP, typename E0, typename E1Tag>
double Binary<OP, E0, Const<E1Tag> >::pri = 0;

template<typename OP, typename E0, typename E1Tag>
double Binary<OP, E0, Const<E1Tag> >::adj = 0;




template<typename T> struct PrimalEvaluator {};

template<typename OP, typename E0, typename E1>
struct PrimalEvaluator<Binary<OP, E0, E1> >
  : virtual Binary<OP, PrimalEvaluator<E0>, PrimalEvaluator<E1> >
{
  PrimalEvaluator() { reinterpret_cast<Binary<OP, E0, E1> const *>(this)->primal(); }
};

template<typename OP, typename E0, typename E1Tag>
struct PrimalEvaluator<Binary<OP, E0, Const<E1Tag> > >
  : virtual Binary<OP, PrimalEvaluator<E0>, Const<E1Tag> >
{
  PrimalEvaluator() { reinterpret_cast<Binary<OP, E0, Const<E1Tag> > const *>(this)->primal(); }
};

template<typename Tag>
struct PrimalEvaluator<Terminal<Tag> >
  : virtual Terminal<Tag>
{
  PrimalEvaluator() { reinterpret_cast<Terminal<Tag> const *>(this)->primal(); }
};


template<typename T> struct AdjointEvaluator {};

template<typename OP, typename E0, typename E1>
struct AdjointEvaluator<Binary<OP, E0, E1> >
  : virtual Binary<OP, AdjointEvaluator<E0>, AdjointEvaluator<E1> >
{
  ~AdjointEvaluator() { reinterpret_cast<Binary<OP, E0, E1> const *>(this)->adjoint(); }
};

template<typename OP, typename E0, typename E1Tag>
struct AdjointEvaluator<Binary<OP, E0, Const<E1Tag> > >
  : virtual Binary<OP, AdjointEvaluator<E0>, Const<E1Tag> >
{
  ~AdjointEvaluator() { reinterpret_cast<Binary<OP, E0, Const<E1Tag> > const *>(this)->adjoint(); }
};

template<typename Tag>
struct AdjointEvaluator<Terminal<Tag> >
  : virtual Terminal<Tag>
{
  ~AdjointEvaluator() { reinterpret_cast<Terminal<Tag> const *>(this)->adjoint(); }
};




double eval(double a, double b, double c0, double c1)
{
    double ab = a*b;
    double left = ab + c0;
    double right = ab + c1;
    double w = left/right;
    return w;
}

void eval_b(double a, double *ab0, double b, double *bb, double c0, double c1, double *wb)
{
    double ab = (a)*(b);
    double abb = 0.0;
    double left = ab + c0;
    double leftb = 0.0;
    double right = ab + c1;
    double rightb = 0.0;
    leftb = *wb/right;
    rightb = -(left*(*wb)/(right*right));
    *wb = 0.0;
    abb = rightb + leftb;
    *ab0 = *ab0 + (b)*abb;
    *bb = *bb + (a)*abb;
}




struct A;
struct B;
struct C0;
struct C1;

int main()
{
  double a_pri = 3;
  double b_pri = 4;
  double w_pri = 0;

  double a_adj = 0;
  double b_adj = 0;
  double w_adj = 1;

  {
    Terminal<A> a{a_pri, a_adj};
    Terminal<B> b{b_pri, b_adj};

    Const<C0> c0{12};
    Const<C1> c1{6};

    Binary<Mul, decltype(a), decltype(b)>  t{a, b};
    Binary<Add, decltype(t), decltype(c0)> u{t, c0};
    Binary<Add, decltype(t), decltype(c1)> v{t, c1};
    Binary<Div, decltype(u), decltype(v)>  w{u, v};

    // primal
    PrimalEvaluator<decltype(w)>{};
    w_pri = w.pri;
    // w_pri = eval(a_pri, b_pri, c0.value, c1.value);

    // adjoint
    w.adj = w_adj;
    AdjointEvaluator<decltype(w)>{};
    // eval_b(a_pri, &a_adj, b_pri, &b_adj, c0.value, c1.value, &w_adj);
  }

  std::cout << "a     = " << a_pri << std::endl;
  std::cout << "b     = " << b_pri << std::endl;
  std::cout << "w     = " << w_pri << std::endl;
  std::cout << "a-adj = " << a_adj << std::endl;
  std::cout << "b-adj = " << b_adj << std::endl;
}
