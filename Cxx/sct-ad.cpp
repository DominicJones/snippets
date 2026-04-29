/**
 * From: https://github.com/SteveBronder/cppcon2025_autodiff/blob/main/code/sct.cpp
 *
 * Implement a static expression-template-style reverse-mode system.
 *
 * The nodes in this system wrap lambdas that capture their children. Expressions
 * are materialized as lightweight value objects and the reverse sweep walks the
 * graph in breadth-first order without storing an explicit tape, showcasing a
 * compile-time graph encoded via templates.
 */
#include <cmath>
#include <functional>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>
#include <iostream>


struct var {
  var(double x) : value_(x), adjoint_(0) {}
  auto val() const { return value_; }
  auto &adj() { return adjoint_; }
  double value_;
  double adjoint_;
};

template <typename T>
inline constexpr bool is_var_v = std::is_same_v<std::remove_cvref_t<T>, var>;

template <typename T>
concept Var = is_var_v<std::remove_cvref_t<T>>;

template <typename A, typename B>
concept any_var = Var<A> || Var<B>;


template <typename T> struct deduce_ownership {
  static constexpr bool value = std::is_rvalue_reference_v<T>;
  using type = std::conditional_t<value,
                                  std::remove_reference_t<T>,
                                  std::reference_wrapper<std::decay_t<T>>>;
};

template <typename T>
using deduce_ownership_t = typename deduce_ownership<T &&>::type;


template <typename F, typename... Exprs> struct ad_expr {
  template <typename FF, typename... Args>
  ad_expr(double x, FF &&f, Args &&...args)
      : ret_(x), f_(std::forward<F>(f)), exprs_(std::forward<Args>(args)...) {}
  auto val() { return ret_.val(); }
  auto &adj() { return ret_.adj(); }
  var ret_;
  std::tuple<deduce_ownership_t<Exprs>...> exprs_;
  std::decay_t<F> f_;
};


template <typename T, typename F, typename... Args>
inline auto make_expr(T &&x, F &&f, Args &&...args) {
  return ad_expr<F, Args &&...>{std::forward<T>(x),
                                std::forward<F>(f),
                                std::forward<Args>(args)...};
}


namespace detail {
template <typename T> struct is_expr : std::false_type {};
template <typename F, typename... Exprs>
struct is_expr<ad_expr<F, Exprs...>> : std::true_type {};
template <typename T> struct is_ref_wrap : std::false_type {};
template <typename T>
struct is_ref_wrap<std::reference_wrapper<T>> : std::true_type {};
template <typename T> struct is_ref_wrap_expr : std::false_type {};
template <typename F, typename... Exprs>
struct is_ref_wrap_expr<std::reference_wrapper<ad_expr<F, Exprs...>>> : std::true_type {};
} // namespace detail

template <typename T> struct is_expr : detail::is_expr<std::decay_t<T>> {};
template <typename T> constexpr bool is_expr_v = is_expr<T>::value;

template <typename T>
struct is_ref_wrap : detail::is_ref_wrap<std::decay_t<T>> {};
template <typename T> constexpr bool is_ref_wrap_v = is_ref_wrap<T>::value;
template <typename T>
concept RefWrap = is_ref_wrap_v<T>;

template <typename T>
struct is_ref_wrap_expr : detail::is_ref_wrap_expr<std::decay_t<T>> {};
template <typename T>
constexpr bool is_ref_wrap_expr_v = is_ref_wrap_expr<T>::value;

template <typename T>
concept Expr = is_expr_v<T>;


template <Var T> auto value(T &&x) { return x.val(); }
template <Expr T> auto value(T &&x) { return x.val(); }
template <RefWrap T> auto value(T &&x) { return value(x.get()); }

template <Var T> auto &adjoint(T &&x) { return x.adj(); }
template <Expr T> auto &adjoint(T &&x) { return x.adj(); }
template <RefWrap T> auto &adjoint(T &&x) { return adjoint(x.get()); }

template <typename... Types>
concept any_var_or_expr = ((Var<Types> || Expr<Types>) || ... || (false));


template <typename T1, typename T2>
  requires any_var_or_expr<T1, T2>
inline auto operator+(T1 &&lhs, T2 &&rhs) {
  return make_expr(
      value(lhs) + value(rhs),
      [](auto &&ret, auto &&lhs, auto &&rhs) {
        if constexpr (!std::is_arithmetic_v<T1>) {
          adjoint(lhs) += adjoint(ret);
        }
        if constexpr (!std::is_arithmetic_v<T2>) {
          adjoint(rhs) += adjoint(ret);
        }
      },
      std::forward<T1>(lhs), std::forward<T2>(rhs));
}


template <typename T1, typename T2>
  requires any_var_or_expr<T1, T2>
inline auto operator*(T1 &&lhs, T2 &&rhs) {
  return make_expr(
      value(lhs) * value(rhs),
      [](auto &&ret, auto &&lhs, auto &&rhs) {
        if constexpr (!std::is_arithmetic_v<T1>) {
          adjoint(lhs) += adjoint(ret) * value(rhs);
        }
        if constexpr (!std::is_arithmetic_v<T2>) {
          adjoint(rhs) += adjoint(ret) * value(lhs);
        }
      },
      std::forward<T1>(lhs), std::forward<T2>(rhs));
}


template <typename Expr> inline auto log(Expr &&x) {
  return make_expr(
      std::log(x.val()),
      [](auto &&ret, auto &&x) { adjoint(x) += ret.adj() / value(x); },
      std::forward<Expr>(x));
}


template <typename T> constexpr auto make_expr_tuple(T &&x) {
  if constexpr (is_ref_wrap_expr_v<std::decay_t<T>>) {
    return std::tuple{x};
  } else if constexpr (is_expr_v<std::decay_t<T>>) {
    return std::tuple<std::reference_wrapper<std::decay_t<T>>>{std::ref(x)};
  } else {
    return std::tuple<>();
  }
}


template <typename E> constexpr auto child_exprs(E &&e) {
  static_assert(is_expr_v<std::decay_t<E>> || is_ref_wrap_v<std::decay_t<E>>,
                "child_exprs expects an ad_expr node");
  return std::apply(
      [](auto &...args) { return std::tuple_cat(make_expr_tuple(args)...); },
      std::forward<E>(e).exprs_);
}


template <typename Tuple> constexpr auto bfs_flatten(Tuple &&level) {
  if constexpr (std::tuple_size_v<std::decay_t<Tuple>> == 0) {
    return std::tuple<>();
  } else {
    auto next = std::apply(
        [](auto &...nodes) {
          return std::tuple_cat(child_exprs(nodes.get())...);
        },
        level);
    return std::tuple_cat(std::forward<Tuple>(level),
                          bfs_flatten(std::move(next)));
  }
}


template <typename Expr> constexpr auto collect_bfs(Expr &&z) {
  if constexpr (is_expr_v<std::decay_t<Expr>>) {
    return bfs_flatten(std::tuple{std::ref(std::forward<Expr>(z))});
  } else {
    return std::tuple<>();
  }
}


template <typename Tuple> inline void eval_breadthwise(Tuple &&nodes) {
  std::apply(
      [](auto &...node_wrappers) {
        (std::apply(
             [&](auto &...args) {
               auto &node = node_wrappers.get();
               compute_f(node, args...);
             },
             node_wrappers.get().exprs_),
         ...);
      },
      std::forward<Tuple>(nodes));
}


template <typename Expr, typename... Exprs>
inline auto compute_f(Expr &&z, Exprs &&...exprs) {
  z.f_(z.ret_, exprs...);
}

template <typename... Exprs>
inline constexpr auto compute_f(var &z, Exprs &&...exprs) {}

template <typename... Exprs>
inline constexpr auto compute_f(const var &z, Exprs &&...exprs) {}


template <typename Expr> inline void grad(Expr &&z) {
  auto nodes = collect_bfs(z);
  eval_breadthwise(nodes);
}


int main()
{
    var x(2.0);
    var y(4.0);

    auto z = x * log(y) + log(x * y) * y;

    adjoint(z) = 1;
    grad(z);

    std::cout<<value(z)<<std::endl;
    std::cout<<x.adj()<<std::endl;
    std::cout<<y.adj()<<std::endl;
}
