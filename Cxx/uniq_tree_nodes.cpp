// -*- C++ -*-

#ifndef _hash_combine_h_
#define _hash_combine_h_

#include <cstdint>

constexpr uint64_t hash(uint64_t const &v)
{
  return v * uint64_t(104723);
}

constexpr uint64_t hash_combine(uint64_t const &seed, uint64_t const &v)
{
  return seed ^ (hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

template<typename... Args>
constexpr uint64_t hash_combine(uint64_t const &seed, uint64_t const &v, Args... args)
{
  return hash_combine(hash_combine(seed, args...), v);
}

#endif // _hash_combine_h_


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
#include <typeinfo>
#include <type_traits>
#include <array>
#include <tuple>
#include <utility>
#include <cstring>

struct Pos { static char const * name() { return "+"; } };
struct Neg { static char const * name() { return "-"; } };
struct Mul { static char const * name() { return "*"; } };
struct Div { static char const * name() { return "/"; } };

template<uint64_t v> struct Uniq { static char const * name() { return "uq"; } };

struct A { static char const * name() { return "A"; } };
struct B { static char const * name() { return "B"; } };
struct C { static char const * name() { return "C"; } };


template<typename T>
struct Terminal
  : std::integral_constant<uint64_t, 0>
{
  T v;
};

template<typename F, typename L>
struct Unary
  : std::integral_constant<uint64_t, (L::value + 1)>
{
  Unary(L const &left) : left(left) {}
  void eval() const {}
  L const left;
};

template<typename F, typename L, typename R>
struct Binary
  : std::integral_constant<uint64_t, ((L::value > R::value? L::value: R::value) + 1)>
{
  Binary(L const &left, R const &right) : left(left), right(right) {}
  void eval() const {}
  L const left;
  R const right;
};


template<uint64_t v, typename L>
struct Unary<Uniq<v>, L>
  : std::integral_constant<uint64_t, (L::value)>
{
  Unary(L const &left) : left(left) {}
  L const left;
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
  os << "(" << std::string{F::name()} << expr.left << ")";
  return os;
}

template<typename F, typename L, typename R>
std::ostream &operator<<(std::ostream &os, Binary<F, L, R> const &expr)
{
  os << "(" << std::string{F::name()} << expr.left << "," << expr.right << ")";
  return os;
}


template<typename L>
Unary<Pos, typename std::remove_reference<L>::type>
operator+(L&& l)
{
  return {std::forward<L>(l)};
}

template<typename L>
Unary<Neg, typename std::remove_reference<L>::type>
operator-(L&& l)
{
  return {std::forward<L>(l)};
}

template<typename L, typename R>
Binary<Mul, typename std::remove_reference<L>::type, typename std::remove_reference<R>::type>
operator*(L&& l, R&& r)
{
  return {std::forward<L>(l), std::forward<R>(r)};
}

template<typename L, typename R>
Binary<Div, typename std::remove_reference<L>::type, typename std::remove_reference<R>::type>
operator/(L&& l, R&& r)
{
  return {std::forward<L>(l), std::forward<R>(r)};
}


template<typename Expr, uint64_t v>
Expr propagate_uniq(Expr const &expr, Uniq<v>)
{
  return expr;
}

template<typename F, typename L, uint64_t v>
auto propagate_uniq(Unary<F, L> const &expr, Uniq<v>)
  -> Unary<Uniq<v>, Unary<F,
       decltype(propagate_uniq(expr.left, Uniq<hash_combine(v, L::value, 653)>{}))> >
{
  auto constexpr l1 = hash_combine(v, L::value, 653);
  using L1 = decltype(propagate_uniq(expr.left, Uniq<l1>{}));
  using T = Unary<F, L1>;
  return  Unary<Uniq<v>, T>{propagate_uniq(expr.left, Uniq<l1>{})};
}

template<typename F, typename L, typename R, uint64_t v>
auto propagate_uniq(Binary<F, L, R> const &expr, Uniq<v>)
  -> Unary<Uniq<v>, Binary<F,
       decltype(propagate_uniq(expr.left, Uniq<hash_combine(v, L::value, 809)>{})),
       decltype(propagate_uniq(expr.right, Uniq<hash_combine(v, R::value, 743)>{}))> >
{
  auto constexpr l1 = hash_combine(v, L::value, 809);
  auto constexpr r1 = hash_combine(v, R::value, 743);
  using L1 = decltype(propagate_uniq(expr.left, Uniq<l1>{}));
  using R1 = decltype(propagate_uniq(expr.right, Uniq<r1>{}));
  using T = Binary<F, L1, R1>;
  return  Unary<Uniq<v>, T>{T{propagate_uniq(expr.left, Uniq<l1>{}), propagate_uniq(expr.right, Uniq<r1>{})}};
}


template<typename Expr>
Expr prune_uniq(Expr const &expr)
{
  return expr;
}

template<typename F, typename L>
auto prune_uniq(Unary<F, L> const &expr)
  -> Unary<F, decltype(prune_uniq(L{expr.left}))>
{
  using L1 = decltype(prune_uniq(L{expr.left}));
  return Unary<F, L1>{prune_uniq(L{expr.left})};
}

template<typename F, typename L, typename R>
auto prune_uniq(Binary<F, L, R> const &expr)
  -> Binary<F, decltype(prune_uniq(L{expr.left})), decltype(prune_uniq(R{expr.right}))>
{
  using L1 = decltype(prune_uniq(L{expr.left}));
  using R1 = decltype(prune_uniq(R{expr.right}));
  return Binary<F, L1, R1>{prune_uniq(L{expr.left}), prune_uniq(R{expr.right})};
}

template<typename F, typename L, uint64_t v>
auto prune_uniq(Unary<Uniq<v>, Unary<F, L> > const &expr)
  -> decltype(prune_uniq(Unary<F, L>{expr.left}))
{
  return prune_uniq(Unary<F, L>{expr.left});
}

template<typename F, typename L, typename R, uint64_t v>
auto prune_uniq(Unary<Uniq<v>, Binary<F, L, R> > const &expr)
  -> decltype(prune_uniq(Binary<F, L, R>{expr.left}))
{
  return prune_uniq(Binary<F, L, R>{expr.left});
}

// do not prune active-passive binary nodes
template<typename F, typename L, uint64_t v>
auto prune_uniq(Unary<Uniq<v>, Binary<F, L, Terminal<C> > > const &expr)
  -> Unary<Uniq<v>, Binary<F, decltype(prune_uniq(L{expr.left.left})), Terminal<C> > >
{
  using LL1 = decltype(prune_uniq(L{expr.left.left}));
  using LR1 = Terminal<C>;
  using L1 = Binary<F, LL1, LR1>;
  return Unary<Uniq<v>, L1>{L1{LL1{prune_uniq(L{expr.left.left})}, LR1{expr.left.right}}};
}


int main()
{
  // active terms
  Terminal<A> a;
  Terminal<B> b;

  // passive term
  Terminal<C> c;

  auto t = -a / b;
  auto u = t * c;
  auto v = t * c;
  auto r = u / v;

  auto s1 = Uniq<hash(r.value)>{};
  auto t1 = propagate_uniq(r, s1);
  auto t2 = prune_uniq(t1);

  std::cout << r << std::endl;
  std::cout << t1 << std::endl;
  std::cout << t2 << std::endl;

  std::cout << std::endl;
  std::cout << demangle(t2) << std::endl;
}
