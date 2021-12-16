// C++14

#include <iostream>
#include <type_traits>
#include <utility>


struct Primal {};
struct Adjoint {};


template<typename> struct SelectMode;

template<> struct SelectMode<Primal>
{
  template<typename T> static int eval(T v) { return v + v; }
};
    
template<> struct SelectMode<Adjoint>
{
  template<typename T> static int eval(T v) { return v * v; }
};



template<typename T1, typename T2, typename F> struct Lambda
{
  F fn;
  Lambda(F f) : fn(f) { fn(T1{}); }
  ~Lambda() { fn(T2{}); }
};


template<typename T1, typename T2, typename F>
auto makeLambda(T1, T2, F f) { return Lambda<T1, T2, F>{f}; }


#define Block_begin() \
  auto lambda = makeLambda(Primal{}, Adjoint{}, [&](auto vv) \
  { \
    auto eval = [vv](auto v) \
    { \
      return SelectMode<decltype(vv)>::eval(v); \
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
