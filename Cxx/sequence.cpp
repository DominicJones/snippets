#include <iostream>


namespace slow
{
  template<int...> struct seq {};

  template<class S> struct concat;

  template<int... I>
  struct concat<seq<I...> >
  {
    using type = seq<0, (1 + I)...>;
  };

  template<class S> using concat_t = typename concat<S>::type;


  template<int N> struct make_seq;
  template<int N> using make_seq_t = typename make_seq<N>::type;


  template<> struct make_seq<0> { using type = seq<>; };

  template<int N>
  struct make_seq
  {
    using type = concat_t<make_seq_t<(N-1)> >;
  };
}


namespace fast
{
  template<int...> struct seq {};

  template<class S1, class S2> struct concat;

  template<int... I1, int... I2>
  struct concat<seq<I1...>, seq<I2...> >
  {
    using type = seq<I1..., (sizeof...(I1) + I2)...>;
  };

  template<class S1, class S2> using concat_t = typename concat<S1, S2>::type;


  template<int N> struct make_seq;
  template<int N> using make_seq_t = typename make_seq<N>::type;


  template<> struct make_seq<0> { using type = seq<>; };
  template<> struct make_seq<1> { using type = seq<0>; };

  template<int N>
  struct make_seq
  {
    using type = concat_t<make_seq_t<(N/2)>, make_seq_t<(N-N/2)> >;
  };
}


int main()
{
  {
    slow::make_seq_t<10> s;
    fast::make_seq_t<10> f;
  }
}
