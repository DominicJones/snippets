// -*- C++ -*-

#ifndef _demangle_h_
#define _demangle_h_

#include <typeinfo>
#include <iostream>
#include <cstdlib>
#include <memory>

#if defined(__GNUG__)
#include <cxxabi.h>
struct Demangle
{
  static std::string eval(char const * name)
  {
    int status(-4);
    char * realname;

    realname = abi::__cxa_demangle(name, 0, 0, &status);

    std::string result(realname);
    free(realname);
    return result;
  }
};

template<typename Expr_t>
std::string demangle(Expr_t const &expr)
{
  return Demangle::eval(typeid(expr).name());
}

template<typename Expr_t>
std::string demangle()
{
  return Demangle::eval(typeid(Expr_t).name());
}
#endif

#endif // _demangle_h_


#include <iostream>


struct A {};
struct B {};
struct D {};
struct C {};

struct Pos { static char const * name() { return "+"; } };
struct Neg { static char const * name() { return "-"; } };
struct Add { static char const * name() { return "+"; } };
struct Sub { static char const * name() { return "-"; } };
struct Mul { static char const * name() { return "*"; } };
struct Div { static char const * name() { return "/"; } };
struct Sin { static char const * name() { return "sin"; } };
struct Cos { static char const * name() { return "cos"; } };
struct Tan { static char const * name() { return "tan"; } };
struct Idx { static char const * name() { return "idx"; } };


template<typename F, typename L>
struct Unary { L l; };

template<typename F, typename L, typename R>
struct Binary { L l; R r; };


template<typename L>
auto operator+(L l) { return Unary<Pos, L>{l}; }

template<typename L>
auto operator-(L l) { return Unary<Neg, L>{l}; }

template<typename L>
auto sin(L l) { return Unary<Sin, L>{l}; }

template<typename L>
auto cos(L l) { return Unary<Cos, L>{l}; }

template<typename L>
auto tan(L l) { return Unary<Cos, L>{l}; }


template<typename L, typename R>
auto operator+(L l, R r) { return Binary<Add, L, R>{l, r}; }

template<typename L, typename R>
auto operator-(L l, R r) { return Binary<Sub, L, R>{l, r}; }

template<typename L, typename R>
auto operator/(L l, R r) { return Binary<Div, L, R>{l, r}; }

template<typename L, typename R>
auto operator*(L l, R r) { return Binary<Mul, L, R>{l, r}; }

template<typename L, typename R>
auto idx(L l, R r) { return Binary<Idx, L, R>{l, r}; }


template<class E> auto transform(E e)
{
  // std::cout << "A " << demangle(e) << std::endl;
  return e;
}

template<class F, class L, class R>
auto transform(Binary<F, L, R> e)
{
  // std::cout << "B " << demangle(e) << std::endl;
  return Binary<F, decltype(transform(e.l)),
                   decltype(transform(e.r))>
    {transform(e.l), transform(e.r)};
}

template<class LF, class LL, class LR, class R>
auto transform(Binary<Idx, Binary<LF, LL, LR>, R> e)
{
  // std::cout << "C " << demangle(e) << std::endl;
  return transform(Binary<LF, Binary<Idx, decltype(transform(e.l.l)), R>,
                   Binary<Idx, decltype(transform(e.l.r)), R> >
                     {{transform(e.l.l), transform(e.r)},
                      {transform(e.l.r), transform(e.r)}});
}


template<typename F, typename L, typename R>
std::ostream &operator<<(std::ostream &os, Binary<F, L, R> e)
{
  os << "(" << F::name() << " " << e.l << " " << e.r << ")";
  return os;
}

template<typename F, typename L>
std::ostream &operator<<(std::ostream &os, Unary<F, L> e)
{
  os << "(" << F::name() << " " << e.l << ")";
  return os;
}


int main()
{
  // idx((A + B), i)
  {
    Binary<Add, A, B> _ab;
    Binary<Add, C, D> _cd;
    auto _abcd = _ab - _cd;
    auto r = idx(_ab, 2);
    std::cout << demangle(r) << std::endl;
    auto s = transform(r);
    std::cout << demangle(s) << std::endl;
  }
}
