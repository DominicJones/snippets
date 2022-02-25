/*
IsComplete does not work if in the same translation unit you:
  1. first declare the type Foo (incomplete)
  2. then test it with IsComplete (the compiler will generate an instantiation of IsComplete<Foo> with value == false)
  3. define Foo
  4. then test gain with IsComplete (the previous instantiation of the template IsComplete<Foo> is used again, thus you have again value == false).
*/
#include <type_traits>
#include <iostream>

template< class... >
using void_t = void;

template<typename, auto x, typename = void>
constexpr bool is_type_complete_v = false;

template<typename T, auto x>
constexpr bool is_type_complete_v
    <T, x, void_t<decltype(sizeof(T))>> = true;

template<typename T, auto x = [](){}> // generate unique type
constexpr bool is_type_complete()
{
  return is_type_complete_v<T, x>;
}

int main()
{
  class Foo; // no definition
  
  if (is_type_complete<Foo>()) //== false, as expected
  { std::cout << "Complete!" << std::endl; }
  
  class Foo {}; // OK, now defined...
  
  if (is_type_complete<Foo>()) //== true - as expected
  { std::cout << "Complete!" << std::endl; }  
}
