// see http://b.atch.se/posts/non-constant-constant-expressions/

#include <type_traits>
#include <iostream>


class Empty {};


constexpr Empty hasDefinition(int);


template<typename T>
struct Instantiate
{
  friend constexpr Empty hasDefinition(T) { return Empty(); }
};


template<bool state = noexcept(hasDefinition(int())),
         typename T = typename std::conditional<state, float, int>::type,
         int = sizeof(Instantiate<T>)>
constexpr int evaluate()
{
  return int(state);
}


int main()
{
  constexpr int a = evaluate();
  constexpr int b = evaluate();

  std::cout << "a = " << a << std::endl; // a = 0
  std::cout << "b = " << b << std::endl; // b = 1
}
