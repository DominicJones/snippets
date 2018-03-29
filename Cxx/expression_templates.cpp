#include <type_traits>


namespace std
{
  template<typename...> struct make_void { using type = void; };
  template<typename... T> using void_t = typename make_void<T...>::type;
}


template<typename, typename = void>
struct Has_Typedef_ExpressionBase : std::false_type {};

template<typename T>
struct Has_Typedef_ExpressionBase<T, std::void_t<typename T::ExpressionBase> > : std::true_type {};


template<typename R, typename E>
class Expression
{
public:
  using ExpressionBase = Expression;
  using Result = R;
  E const &cast() const { return static_cast<E const &>(*this); }
};


template<typename R>
class Scalar
  : public Expression<R, Scalar<R> >
{
public:
  Scalar() : _a0(0) {}
  Scalar(R const &a0) : _a0(a0) {}

  template<typename E0>
  Scalar(E0 const &e0,
         typename std::enable_if<Has_Typedef_ExpressionBase<E0>::value>::type * enable = nullptr)
    : _a0(e0.value())
  {}

  R value() const { return {_a0}; }

private:
  R _a0;
};


template<typename R, typename OP, typename E0, typename E1>
class BinaryOperation
  : public Expression<R, BinaryOperation<R, OP, E0, E1> >
{
public:
  BinaryOperation(Expression<R, E0> const &e0,
                  Expression<R, E1> const &e1)
    : _e0(e0.cast())
    , _e1(e1.cast())
  {}

  R value() const { return {OP::value(_e0.value(), _e1.value())}; }

private:
  /// BUG: referencing causes a segmentation fault (g++ -std=c++11 -O3)
  E0 const &_e0;
  E1 const &_e1;

  // E0 const _e0;
  // E1 const _e1;
};


template<typename... T> using Promote = typename std::common_type<T...>::type;


#define GenerateSpecificBinaryOperator(OperatorType, Operator)  \
  template<typename R>                                          \
  struct OperatorType                                           \
  {                                                             \
    template<typename A0, typename A1>                          \
      static R value(A0 const &a0, A1 const &a1)                \
    {                                                           \
      return {a0 Operator a1};                                  \
    }                                                           \
  };

GenerateSpecificBinaryOperator(Add, +)
GenerateSpecificBinaryOperator(Subtract, -)
GenerateSpecificBinaryOperator(Multiply, *)
GenerateSpecificBinaryOperator(Divide, /)

#undef GenerateSpecificBinaryOperator


#define GenerateSpecificBinaryOperation(OperatorType, Operator)         \
  template<typename E0, typename E1>                                    \
  BinaryOperation                                                       \
  <Promote<typename E0::Result, typename E1::Result>,                   \
   OperatorType<Promote<typename E0::Result, typename E1::Result> >,    \
   E0,                                                                  \
   E1                                                                   \
   >                                                                    \
  Operator(Expression<typename E0::Result, E0> const &e0,               \
           Expression<typename E1::Result, E1> const &e1)               \
  {                                                                     \
    return {e0.cast(), e1.cast()};                                      \
  }

GenerateSpecificBinaryOperation(Add, operator+)
GenerateSpecificBinaryOperation(Subtract, operator-)
GenerateSpecificBinaryOperation(Multiply, operator*)
GenerateSpecificBinaryOperation(Divide, operator/)

#undef GenerateSpecificBinaryOperation


int main()
{
  Scalar<double> const a(3);
  Scalar<double> const b(4);
  Scalar<double> z;

  auto u = a + b * a;
  auto v = a - b / a;
  z = u * v;

  return int(z.value());
}
