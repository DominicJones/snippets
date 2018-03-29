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
struct Adj { static char const * name() { return "~"; } };

struct A { static char const * name() { return "A"; } };
struct B { static char const * name() { return "B"; } };
struct C { static char const * name() { return "C"; } };


template<typename T>
struct Terminal
{
  T value;
};

template<typename F, typename L>
struct Unary
{
  Unary(L const &left) : left(left) {}
  void eval() const {}
  L const left;
};

template<typename F, typename L, typename R>
struct Binary
{
  Binary(L const &left, R const &right) : left(left), right(right) {}
  void eval() const {}
  L const left;
  R const right;
};

template<typename L>
struct Unary<Adj, L>
{
  Unary(L const &left) : left(left) {}
  ~Unary() { left.eval(); }
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


template<typename Expr>
Expr transform(const Expr &expr)
{
  return expr;
}

template<typename F, typename L>
auto transform(Unary<F, L> const &expr)
  -> Unary<Adj, Unary<F, decltype(transform(expr.left))> >
{
  using L1 = decltype(transform(expr.left));
  return  Unary<Adj, Unary<F, L1> >{transform(expr.left)};
}

template<typename F, typename L, typename R>
auto transform(Binary<F, L, R> const &expr)
  -> Unary<Adj, Binary<F, decltype(transform(expr.left)), decltype(transform(expr.right))> >
{
  using L1 = decltype(transform(expr.left));
  using R1 = decltype(transform(expr.right));
  return  Unary<Adj, Binary<F, L1, R1> >{Binary<F, L1, R1>{transform(expr.left), transform(expr.right)}};
}


int main()
{
  Terminal<A> a;
  Terminal<B> b;
  Terminal<C> c;

  {
    auto r = a;
    std::cout << r << " ---> " << transform(r) << std::endl;
  }

  {
    auto r = +a;
    std::cout << r << " ---> " << transform(r) << std::endl;
  }

  {
    auto r = -(+a);
    std::cout << r << " ---> " << transform(r) << std::endl;
  }

  {
    auto r = +(-(+a));
    std::cout << r << " ---> " << transform(r) << std::endl;
  }

  {
    auto r = -a * b / c;
    std::cout << r << " ---> " << transform(r) << std::endl;
  }
}
