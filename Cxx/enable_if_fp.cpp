#include <iostream>
#include <type_traits>


#define EnableIfUnaryFP(E0)                                             \
  std::enable_if_t<(std::is_floating_point<std::remove_const_t<std::remove_reference_t<E0> > >::value), void*> = nullptr

#define EnableIfBinaryFP(E0, E1)                                        \
  std::enable_if_t<(std::is_floating_point<std::remove_const_t<std::remove_reference_t<E0> > >::value && \
                    std::is_floating_point<std::remove_const_t<std::remove_reference_t<E1> > >::value), void*> = nullptr


template<typename A, EnableIfUnaryFP(A)>
auto evaluate(A &&a)
{
  return -a;
}

template<typename A, typename B, EnableIfBinaryFP(A, B)>
auto evaluate(A &&a, B &&b)
{
  return a + b;
}

template<typename A, typename B, EnableIfUnaryFP(A), EnableIfUnaryFP(B)>
auto evaluate_v2(A &&a, B &&b)
{
  return a + b;
}


int main()
{
  float a = 2;
  double b = 3;
  auto c = evaluate(a);
  auto d = evaluate(a, b);
  auto e = evaluate_v2(a, b);
  std::cout << c << std::endl;
  std::cout << d << std::endl;
  std::cout << e << std::endl;
}
