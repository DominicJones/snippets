// C++20

#include <iostream>
#include <type_traits>
#include <utility>


using Eval = std::integral_constant<char, sizeof(char)>;
using EvalAlt = std::integral_constant<int, sizeof(int)>;

template<typename T> int eval(T v) { return v + v; }
template<typename T> int evalAlt(T v) { return v * v; }


template<typename T1, typename T2, typename F> struct CDLambda
{
  F fn;
  CDLambda(F f) : fn(f) { fn(T1{}); }
  ~CDLambda() { fn(T2{}); }
};

template<typename T1, typename T2, typename F>
auto makeCDLambda(T1, T2, F f) { return CDLambda<T1, T2, F>{f}; }


#define Block_begin() \
  auto lambda = makeCDLambda(Eval::value, EvalAlt::value, [&]<typename TT>(TT vv) \
  { \
    auto eval = [vv]<typename T>(T v) \
    { \
      if constexpr (sizeof(vv) == EvalAlt::value) \
        return ::evalAlt(v); \
      else \
        return ::eval(v); \
    };

#define Block_end() \
  });


int main()
{
  int x = 3;

  Block_begin()
  { 
    std::cout << eval(x) << std::endl;
  }
  Block_end();
}
