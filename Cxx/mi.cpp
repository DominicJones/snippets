
// #include <chrono>

// class Timer
// {
// private:
//   using Resolution = std::chrono::microseconds;

// public:
//   void start()
//   {
//     _start = std::chrono::high_resolution_clock::now();
//   }

//   void stop()
//   {
//     std::chrono::time_point<std::chrono::system_clock> const stop =
//       std::chrono::high_resolution_clock::now();
//     _duration = std::chrono::duration_cast<Resolution>(stop - _start);
//   }

//   double duration() const
//   {
//     return static_cast<double>(_duration.count());
//   }

// private:
//   std::chrono::time_point<std::chrono::system_clock> _start;
//   Resolution _duration;
// };




// #include <typeinfo>
// #include <iostream>
// #include <cstdlib>
// #include <memory>

// #if defined(__GNUG__)
// #include <cxxabi.h>
// struct Demangle
// {
//   static std::string eval(char const * name)
//   {
//     int status(-4);
//     char * realname;

//     realname = abi::__cxa_demangle(name, 0, 0, &status);

//     std::string result(realname);
//     free(realname);
//     return result;
//   }
// };

// template<typename Expr_t>
// std::string demangle(Expr_t const &expr)
// {
//   return Demangle::eval(typeid(expr).name());
// }

// template<typename Expr_t>
// std::string demangle()
// {
//   return Demangle::eval(typeid(Expr_t).name());
// }
// #endif




struct Add
{
  static double eval(double a, double b) { return a + b; }
  static void diff(double a, double &ad, double b, double &bd, double rd) { ad += rd; bd += rd; }
};

struct Sub
{
  static double eval(double a, double b) { return a - b; }
  static void diff(double a, double &ad, double b, double &bd, double rd) { ad += rd; bd -= rd; }
};

struct Mul
{
  static double eval(double a, double b) { return a * b; }
  static void diff(double a, double &ad, double b, double &bd, double rd) { ad += b * rd; bd += a * rd; }
};

struct Div
{
  static double eval(double a, double b) { return a / b; }
  static void diff(double a, double &ad, double b, double &bd, double rd) { ad += rd / b; bd -= (a * rd) / (b * b); }
};




template<typename T>
struct Terminal
{
  Terminal(double const &v)
  {
    this->v = v;
    this->d = 0;
  }

  Terminal() {}

  void eval() const {}

  void diff() const {}

  static double v;
  static double d;
};

template<typename T>
double Terminal<T>::v = 0;

template<typename T>
double Terminal<T>::d = 0;


template<typename OP, typename L, typename R>
struct Binary: virtual L, virtual R
{
  Binary(L const &l, R const &r)
    : L(l), R(r)
  {}

  Binary() {}

  void eval() const
  {
    L const &l(static_cast<L const &>(*this));
    R const &r(static_cast<R const &>(*this));
    l.eval();
    r.eval();
    this->v = OP::eval(l.v, r.v);
  }

  void diff() const
  {
    L const &l(static_cast<L const &>(*this));
    R const &r(static_cast<R const &>(*this));
    OP::diff(l.v, l.d, r.v, r.d, this->d);
  }

  static double v;
  static double d;
};

template<typename OP, typename L, typename R>
double Binary<OP, L, R>::v = 0;

template<typename OP, typename L, typename R>
double Binary<OP, L, R>::d = 0;


template<typename OP, typename L>
struct Binary<OP, L, double>: virtual L
{
  Binary(L const &l, double const &r)
    : L(l)
  {
    this->rv = r;
  }

  Binary() {}

  void eval() const
  {
    L const &l(static_cast<L const &>(*this));
    l.eval();
    this->v = OP::eval(l.v, rv);
  }

  void diff() const
  {
    L const &l(static_cast<L const &>(*this));
    double rvd{0};
    OP::diff(l.v, l.d, rv, rvd, this->d);
  }

  static double rv;
  static double v;
  static double d;
};

template<typename OP, typename L>
double Binary<OP, L, double>::rv = 0;

template<typename OP, typename L>
double Binary<OP, L, double>::v = 0;

template<typename OP, typename L>
double Binary<OP, L, double>::d = 0;




template<class T> struct Diff {};

template<class OP, class L, class R>
struct Diff<Binary<OP, L, R> >
  : virtual Binary<OP, Diff<L>, Diff<R> >
{
  ~Diff()
  {
    reinterpret_cast<Binary<OP, L, R> const *>(this)->diff();
  }
};

template<class OP, class L>
struct Diff<Binary<OP, L, double> >
  : virtual Binary<OP, Diff<L>, double>
{
  ~Diff()
  {
    reinterpret_cast<Binary<OP, L, double> const *>(this)->diff();
  }
};




#include <iostream>

struct A;
struct B;

int main()
{
  double volatile _a = 3;
  double volatile _b = 4;
  double volatile _rd = 1;

  int volatile n_it = 1;
  // int volatile n_it = 100000;

  Terminal<A> a{double(_a)};
  Terminal<B> b{double(_b)};

  double r = 0;

  // Timer timer;
  // timer.start();
  for (int it = 0; it != n_it; ++it)
  {
    Binary<Mul, decltype(a), decltype(b)> t{a, b};
    Binary<Add, decltype(t), double>      u{t, double{6}};
    Binary<Sub, decltype(t), double>      v{t, double{6}};
    Binary<Div, decltype(u), decltype(v)> w{u, v};

    // primal
    w.eval();
    r = w.v;
  }
  // timer.stop();
  // double tr = timer.duration();

  // timer.start();
  for (int it = 0; it != n_it; ++it)
  {
    Binary<Mul, decltype(a), decltype(b)> t{a, b};
    Binary<Add, decltype(t), double>      u{t, double{6}};
    Binary<Sub, decltype(t), double>      v{t, double{6}};
    Binary<Div, decltype(u), decltype(v)> w{u, v};

    // primal
    w.eval();
    r = w.v;

    // adjoint
    w.d = double(_rd);
    Diff<decltype(w)>{};
  }
  // timer.stop();
  // double trd = timer.duration();

  // std::cout << tr << std::endl;
  // std::cout << trd << std::endl;
  // std::cout << trd / tr << std::endl;

  std::cout << "a       = " << a.v << std::endl;
  std::cout << "b       = " <<  b.v << std::endl;
  std::cout << "result  = " <<  r << std::endl;
  std::cout << "a-adj   = " <<  a.d << std::endl;
  std::cout << "b-adj   = " <<  b.d << std::endl;

  return int{r + a.d + b.d};
}
