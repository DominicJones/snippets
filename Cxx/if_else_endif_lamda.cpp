#include <iostream>
#include <utility>
#include <type_traits>

#define JOIN(token1, token2) JOIN_IMPL(token1, token2)
#define JOIN_IMPL(token1, token2) token1 ## token2


template<typename> struct is_const_ref { static auto constexpr value = false; };
template<typename T> struct is_const_ref<T const &> { static auto constexpr value = true; };

struct Primal;
struct Adjoint;


template<typename M, typename T> struct Drv;

template<typename T> struct Drv<Primal, T>
{
  T primal;
  template<typename E> void operator=(E &&e) {
    this->primal = evalPrimal(e);
  }
};

template<typename T> struct Drv<Adjoint, T>
{
  T primal;
  T adjoint = 0;
  template<typename E> void operator=(E &&e) {
    this->primal = evalPrimal(e);
    if (this->adjoint != 0)
      evalAdjoint(e, this->adjoint);
  }
};


template<typename F, typename... Args>
auto makeLambda(F &&f, Args &&... f_args) noexcept
{
  return [=]() -> std::result_of_t<F(Args...)>
  {
    return f(std::forward<Args>(f_args)...);
  };
}

int rc_incr = 0;
int rc_decr = 0;

template<typename F>
auto controlFlow_If(bool cond, F &&f) noexcept
{
  struct ControlFlow_If
  {
    using Lambda = decltype(makeLambda(std::move(f)));
    Lambda lambda;

    ControlFlow_If(bool cond, F &&fn)
      : cond(cond)
      , lambda{makeLambda(std::move(fn))}
    {
      if (cond) lambda();
    }

    ~ControlFlow_If()
    {
      if (cond) lambda();
    }

    bool cond;
  };

  return ControlFlow_If{cond, std::move(f)};
}


#define CONTROLFLOW_IF(cond)                                            \
  bool controlflow_cond = cond;                                         \
  auto JOIN(lambda, __LINE__) = controlFlow_If(cond, [&]

#define CONTROLFLOW_ELSE_IF(cond)                                       \
  );                                                                    \
  bool const JOIN(controlflow_else, __LINE__) = (!controlflow_cond && cond);  { controlflow_cond = controlflow_cond || cond; }  auto JOIN(lambda, __LINE__) = controlFlow_If(JOIN(controlflow_else, __LINE__), [&]

#define CONTROLFLOW_ELSE                                                \
  );                                                                    \
  auto JOIN(lambda, __LINE__) = controlFlow_If(!controlflow_cond, [&]

#define CONTROLFLOW_ENDIF                       \
  );


struct Mul;

template<typename OP, typename A, typename B> struct Binary { A a; B b; int rc = 0; };

template<typename T> auto rc_value(T &&e) { return 0; }

template<typename OP, typename A, typename B>
auto rc_value(Binary<OP, A, B> const &e) { return e.rc; }

template<typename T> void rc_incr(T &&e) {}

template<typename OP, typename A, typename B>
void rc_incr(Binary<OP, A, B> const &e) { const_cast<Binary<OP, A, B> &>(e).rc++; }

template<typename T> void rc_decr(T &&e) {}

template<typename OP, typename A, typename B>
void rc_decr(Binary<OP, A, B> const &e) { const_cast<Binary<OP, A, B> &>(e).rc--; }


template<typename A, typename B>
 auto operator*(A &&a, B &&b) {
  return Binary<Mul, A, B>{std::forward<A>(a), std::forward<B>(b)};
}


template<typename M, typename T>
auto evalPrimal(Drv<M, T> const &e) {
  return e.primal;
}

template<typename A, typename B>
auto evalPrimal(Binary<Mul, A, B> const &e) {
  rc_incr(e.a);
  rc_incr(e.b);
  return evalPrimal(e.a) * evalPrimal(e.b);
}


template<typename M, typename T, typename RHS>
void evalAdjoint(Drv<M, T> const &e, RHS rhs) {
  const_cast<Drv<M, T> &>(e).adjoint = rhs;
}

template<typename A, typename B, typename RHS>
void evalAdjoint(Binary<Mul, A, B> const &e, RHS rhs) {
  auto a = evalPrimal(e.a);
  auto b = evalPrimal(e.b);
  auto a_rhs = b * rhs;
  auto b_rhs = a * rhs;
  evalAdjoint(e.a, a_rhs);
  evalAdjoint(e.b, b_rhs);
  rc_decr(e.a);
  rc_decr(e.b);
}


// using Mode = Primal;
using Mode = Adjoint;
using Active = Drv<Mode, float>;


void eval(Active const &x1, Active const &x2, Active &y)
{
  auto const x = x1 * x2;

  Active y1;
  CONTROLFLOW_IF(true)
  {
    auto const xp = x1 * x;
    y1 = xp * x;
  }
  CONTROLFLOW_ELSE
  {
    auto const xp = x1 * x2 * x;
    y1 = xp * x;
  }
  CONTROLFLOW_ENDIF;

  y = y1 * x2;
}


template<class> struct debug_t;

int main()
{
  Active x1{3,0};
  Active x2{4,0};
  Active y{0,1};

  eval(x1, x2, y);

  std::cout << y.primal << std::endl;
  std::cout << x1.adjoint << std::endl;
  std::cout << x2.adjoint << std::endl;
}
