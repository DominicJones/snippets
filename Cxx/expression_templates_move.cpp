#include <type_traits>


template<typename... T> using Promote = typename std::common_type<T...>::type;


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
};


// scalar expression template implementation

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

  template<typename E0>
  typename std::enable_if<Has_Typedef_ExpressionBase<E0>::value>::type
  operator+=(E0 const &e0) { _a0 += e0.value(); }

  R value() const { return {_a0}; }

private:
  R _a0;
};


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


#define GenerateBinaryOperation(Suffix, A0_Param, A0_Arg, A1_Param, A1_Arg) \
  template<typename R, typename OP, typename E0, typename E1>           \
  class BinaryOperation ## Suffix                                       \
    : public Expression<R, BinaryOperation ## Suffix <R, OP, E0, E1> >  \
    {                                                                   \
    public:                                                             \
      BinaryOperation ## Suffix (Expression<R, E0> A0_Param e0,         \
                                 Expression<R, E1> A1_Param e1)         \
        : _e0(static_cast<E0 A0_Param >(e0))                            \
        , _e1(static_cast<E1 A1_Param >(e1))                            \
      {}                                                                \
                                                                        \
        R value() const { return {OP::value(_e0.value(), _e1.value())}; } \
                                                                        \
    private:                                                            \
        E0 A0_Arg _e0;                                                  \
        E1 A1_Arg _e1;                                                  \
    };

GenerateBinaryOperation(RR, const &, const &, const &, const &)
GenerateBinaryOperation(MR, const &&, const, const &, const &)
GenerateBinaryOperation(RM, const &, const &, const &&, const)
GenerateBinaryOperation(MM, const &&, const, const &&, const)

#undef GenerateBinaryOperation


#define GenerateSpecificBinaryOperation(Suffix, A0_Param, A1_Param, OperatorType, Operator) \
  template<typename E0, typename E1>                                    \
  BinaryOperation ## Suffix                                             \
  <Promote<typename E0::Result, typename E1::Result>,                   \
   OperatorType<Promote<typename E0::Result, typename E1::Result> >,    \
   E0,                                                                  \
   E1                                                                   \
   >                                                                    \
  Operator(Expression<typename E0::Result, E0> A0_Param e0,             \
           Expression<typename E1::Result, E1> A1_Param e1)             \
  {                                                                     \
    return                                                              \
    {                                                                   \
      static_cast<E0 A0_Param >(e0),                                    \
        static_cast<E1 A1_Param >(e1)                                   \
        };                                                              \
  }

GenerateSpecificBinaryOperation(RR, const &, const &, Add, operator+)
GenerateSpecificBinaryOperation(MR, const &&, const &, Add, operator+)
GenerateSpecificBinaryOperation(RM, const &, const &&, Add, operator+)
GenerateSpecificBinaryOperation(MM, const &&, const &&, Add, operator+)

GenerateSpecificBinaryOperation(RR, const &, const &, Subtract, operator-)
GenerateSpecificBinaryOperation(MR, const &&, const &, Subtract, operator-)
GenerateSpecificBinaryOperation(RM, const &, const &&, Subtract, operator-)
GenerateSpecificBinaryOperation(MM, const &&, const &&, Subtract, operator-)

GenerateSpecificBinaryOperation(RR, const &, const &, Multiply, operator*)
GenerateSpecificBinaryOperation(MR, const &&, const &, Multiply, operator*)
GenerateSpecificBinaryOperation(RM, const &, const &&, Multiply, operator*)
GenerateSpecificBinaryOperation(MM, const &&, const &&, Multiply, operator*)

GenerateSpecificBinaryOperation(RR, const &, const &, Divide, operator/)
GenerateSpecificBinaryOperation(MR, const &&, const &, Divide, operator/)
GenerateSpecificBinaryOperation(RM, const &, const &&, Divide, operator/)
GenerateSpecificBinaryOperation(MM, const &&, const &&, Divide, operator/)

#undef GenerateSpecificBinaryOperation


// test

#define Evaluate(Suffix, LHS)                                           \
  template<typename T>                                                  \
  void evaluate ## Suffix (int r_size, T const &a, T const &b, T &z)    \
  {                                                                     \
    z = 0;                                                              \
    for (int it = 0; it != r_size; ++it)                                \
    {                                                                   \
      LHS const c = a + b * a;                                          \
      LHS const d = a - b / a;                                          \
      LHS const e = (a - b / a) * (a + b * a);                          \
      LHS const f = (a - b / a) / (a + b * a);                          \
      LHS const g = (c * e) / (d * f);                                  \
      z += a * b * c * d * e * f * g;                                   \
    }                                                                   \
  }

Evaluate(_auto, auto)
Evaluate(_type, T)

#undef Evaluate

template void evaluate_auto<double>(int r_size, double const &a, double const &b, double &z);
template void evaluate_auto<Scalar<double> >(int r_size, Scalar<double> const &a, Scalar<double> const &b, Scalar<double> &z);

template void evaluate_type<double>(int r_size, double const &a, double const &b, double &z);
template void evaluate_type<Scalar<double> >(int r_size, Scalar<double> const &a, Scalar<double> const &b, Scalar<double> &z);


#include <chrono>
class Timer
{
private:
  using Resolution = std::chrono::nanoseconds;

public:
  void start()
  {
    _start = std::chrono::high_resolution_clock::now();
  }

  void stop()
  {
    std::chrono::time_point<std::chrono::system_clock> const stop =
      std::chrono::high_resolution_clock::now();
    _duration = std::chrono::duration_cast<Resolution>(stop - _start);
  }

  double duration() const
  {
    return static_cast<double>(_duration.count());
  }

private:
  std::chrono::time_point<std::chrono::system_clock> _start;
  Resolution _duration;
};


#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
void test_evaluate(int r_size, int t_size)
{
  std::map<std::string, std::vector<double> > durations;

  for (int it = 0; it != t_size; ++it)
  {
    {
      Timer timer;
      timer.start();

      double const a(3);
      double const b(4);
      double z;
      evaluate_auto(r_size, a, b, z);

      timer.stop();
      durations["evaluate-auto-double"].push_back(timer.duration());
    }

    {
      Timer timer;
      timer.start();

      Scalar<double> const a(3);
      Scalar<double> const b(4);
      Scalar<double> z;
      evaluate_auto(r_size, a, b, z);

      timer.stop();
      durations["evaluate-auto-Scalar<double>"].push_back(timer.duration());
    }

    {
      Timer timer;
      timer.start();

      double const a(3);
      double const b(4);
      double z;
      evaluate_type(r_size, a, b, z);

      timer.stop();
      durations["evaluate-type-double"].push_back(timer.duration());
    }

    {
      Timer timer;
      timer.start();

      Scalar<double> const a(3);
      Scalar<double> const b(4);
      Scalar<double> z;
      evaluate_type(r_size, a, b, z);

      timer.stop();
      durations["evaluate-type-Scalar<double>"].push_back(timer.duration());
    }
  }

  for (auto &duration: durations)
  {
    std::sort(duration.second.begin(), duration.second.end());
  }

  double t_ref = 1;
  {
    auto duration = durations.find("evaluate-type-double");

    if (duration != durations.end())
      t_ref = duration->second[duration->second.size()*0.50];
  }

  std::cout << "timings:" << std::endl;
  for (auto const &duration: durations)
  {
    std::cout << std::setprecision(3);
    std::cout << std::scientific;
    std::cout << std::setw(30);

    std::cout << duration.first
              << " | " << duration.second[duration.second.size()*0.50] / t_ref
              << std::endl;
  }
}


int main()
{
  test_evaluate(1000000, 20);
}
