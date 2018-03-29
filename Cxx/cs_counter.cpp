/*
 * taken from:
 * http://www.copperspice.com/pdf/CompileTimeCounter-CppCon-2015.pdf
 */

#include <iostream>

template<int N> struct CSInt: CSInt<N-1> { static constexpr int value = N; };
template<> struct CSInt<0> { static constexpr int value = 0; };


struct Eval
{
  // default "cs_register"
  template<int N> static void cs_register(CSInt<N>){}

  // terminal "cs_counter"
  static constexpr CSInt<0> cs_counter(CSInt<0>);


  // macro expansion from line 42...
  static constexpr int value_42 = decltype(cs_counter(CSInt<255>{}))::value;
  static constexpr CSInt<value_42 + 1> cs_counter(CSInt<value_42 + 1>);
  static void cs_register(CSInt<value_42>)
  {
    std::cout << value_42 << std::endl;
    cs_register(CSInt<value_42 + 1>{});
  }


  // macro expansion from line 43...
  static constexpr int value_43 = decltype(cs_counter(CSInt<255>{}))::value;
  static constexpr CSInt<value_43 + 1> cs_counter(CSInt<value_43 + 1>);
  static void cs_register(CSInt<value_43>)
  {
    std::cout << value_43 << std::endl;
    cs_register(CSInt<value_43 + 1>{});
  }


  // macro expansion from line 44...
  static constexpr int value_44 = decltype(cs_counter(CSInt<255>{}))::value;
  static constexpr CSInt<value_44 + 1> cs_counter(CSInt<value_44 + 1>);
  static void cs_register(CSInt<value_44>)
  {
    std::cout << value_44 << std::endl;
    cs_register(CSInt<value_44 + 1>{});
  }
};


int main()
{
  // recursively calls the "cs_register" methods
  Eval::cs_register(CSInt<0>());
}
