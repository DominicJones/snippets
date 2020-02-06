
#include <type_traits>
#include <tuple>

template<typename F> struct function_traits;

template<typename R, typename... Args>
struct function_traits<R(Args...)>
{
  using return_type = R;

  static constexpr std::size_t arity() { return sizeof...(Args); }

  template<std::size_t N>
  struct argument
  {
    static_assert(N < arity(), "");
    using type = typename std::tuple_element<N, std::tuple<Args...> >::type;
  };
};

template<typename R, typename... Args>
struct function_traits<R(*)(Args...)>
 : public function_traits<R(Args...)>
{};

template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)>
 : public function_traits<R(C&, Args...)>
{
  using class_type = C;

  template<R(C::* method)(Args...)>
  inline static R function(void * const object, Args... args) // noexcept
  {
    return (static_cast<C *>(object)->*method)(args...);
  }
};

template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const>
 : public function_traits<R(C&, Args...)>
{
  using class_type = C;

  template<R(C::* method)(Args...) const>
  inline static R function(void const * const object, Args... args) // noexcept
  {
    return (static_cast<C const *>(object)->*method)(args...);
  }
};

template<typename C, typename R>
struct function_traits<R(C::*)>
 : public function_traits<R(C&)>
{
  using class_type = C;
};


//


template<typename F, F f>
struct function_binding
  : public function_traits<F>
{
  static constexpr F function_address() noexcept { return f; }
};

#define FunctionBinding(FunctionName) function_binding<decltype(&FunctionName), &FunctionName>


//


#include <iostream>
#include <tuple>
#include <type_traits>


struct Primal {};
struct Tangent {};
struct Adjoint {};


template<typename T>
struct ScopedExprFunctionBinding
{
  using type = T;
  using class_type = T;
};

template<typename T>
struct ScopedExprFunctionBinding<ScopedExprFunctionBinding<T> >
{
  using type = T;
  using class_type = typename T::class_type;
};

template<typename T>
using ScopedExprFunctionBinding_t = typename ScopedExprFunctionBinding<T>::type;

template<typename T>
using ScopedExprFunctionBinding_ct = typename ScopedExprFunctionBinding<T>::class_type;


template<typename B, typename... EE>
struct Node
{
  using Binding = B;

  using Object = ScopedExprFunctionBinding_ct<B>;

  Node(Object const &object, EE &&... ee) : _object(object) {}
  Object const &_object;
};

struct Evaluator
{
  template<typename E>
  static void apply(E const &expr)
  {
    using Binding = ScopedExprFunctionBinding_t<typename E::Binding>;
    (expr._object.*(Binding::function_address()))(typename Binding::template argument<1>::type{});
  }
};


struct DynamicDiscriminantBase
{
  void evaluatePrimal(Primal const &) const { std::cout << "primal" << std::endl; }
  void evaluateTangent(Tangent const &) const { std::cout << "tangent" << std::endl; }
  void evaluateAdjoint(Adjoint const &) const { std::cout << "adjoint" << std::endl; }
};


template<typename Mode> struct DynamicDiscriminantBase_evaluate;
template<> struct DynamicDiscriminantBase_evaluate<Primal> : FunctionBinding(DynamicDiscriminantBase::evaluatePrimal) {};
template<> struct DynamicDiscriminantBase_evaluate<Tangent> : FunctionBinding(DynamicDiscriminantBase::evaluateTangent) {};
template<> struct DynamicDiscriminantBase_evaluate<Adjoint> : FunctionBinding(DynamicDiscriminantBase::evaluateAdjoint) {};


template<typename Mode, typename E0>
Node<ScopedExprFunctionBinding<DynamicDiscriminantBase_evaluate<Mode> >, E0>
node(DynamicDiscriminantBase const &op, E0 &&e0)
{
  return {op, std::forward<E0>(e0)};
}


int main()
{
  DynamicDiscriminantBase const op;

  Evaluator::apply(node<Primal>(op, Primal{}));
  Evaluator::apply(node<Tangent>(op, Tangent{}));
  Evaluator::apply(node<Adjoint>(op, Adjoint{}));
}
