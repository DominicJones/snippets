#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <array>
#include <tuple>
#include <utility>


template<bool move, typename T>
typename std::enable_if<move, typename std::remove_reference<T>::type &&>::type
conditional_move(T &&v) { return std::move(v); }

template<bool move, typename T>
typename std::enable_if<!move, T &>::type
conditional_move(T &v) { return v; }


struct Numeric {};

// Cannot use templated 'eval'; specialisation is needed
// template<typename T> void eval(T &value) { std::cout << "Numeric &" << std::endl; }
void eval(Numeric &value) { std::cout << "Numeric &" << std::endl; }

template<typename T> void eval(T &&value) { std::cout << "Numeric &&" << std::endl; }


int main()
{
  Numeric value;
  eval(value);
  eval(Numeric(value));
  eval(std::move(value));
  eval(conditional_move<true>(value));
  eval(conditional_move<false>(value));
}
