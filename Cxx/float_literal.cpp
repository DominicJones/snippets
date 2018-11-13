#include <iostream>


template<char...> struct mp_chars {};


template<auto B, auto E, auto M>
struct multiplier
{
  auto static constexpr value = multiplier<B, E-1, M*10>::value;
};


template<auto B, auto M>
struct multiplier<B, 0, M>
{
  auto static constexpr value = M;
};


template<auto H, auto L, auto E>
struct _float
{
  auto constexpr static value = (H + float(L) / multiplier<10, E, 1>::value);
  constexpr operator auto() const { return value; }
};


template<auto H, auto L, auto E, auto I, auto N, class C>
struct make_float;


template<auto H, auto L, auto E, auto I, auto N, class C> using make_float_t =
  typename make_float<H, L, E, I, N, C>::type;


// terminal
template<auto H, auto L, auto E, auto I, auto N, template<char...> class CL>
struct make_float<H, L, E, I, N, CL<> >
{
  using type = _float<H, L, (N - E)>;
};


// decimal point
template<auto H, auto L, auto E, auto I, auto N, template<char...> class CL, char... Cs>
struct make_float<H, L, E, I, N, CL<'.', Cs...> >
{
  auto static constexpr _E = I + 1;
  using type = make_float_t<H, L, _E, (I+1), N, CL<Cs...> >;
};


// high / low
template<auto H, auto L, auto E, auto I, auto N, template<char, char...> class CL, char C, char... Cs>
struct make_float<H, L, E, I, N, CL<C, Cs...> >
{
  auto static constexpr _D = (C >= '0' && C <= '9');
  auto static constexpr _H = (_D && E == 0)? (10 * H) + (C - '0'): H;
  auto static constexpr _L = (_D && E > 0)?  (10 * L) + (C - '0'): L;
  using type = make_float_t<_H, _L, E, (I+1), N, CL<Cs...> >;
};


// terminal
template<int H, int L, int E, int I, int N, template<char...> class CL, char... Cs>
auto constexpr make_float_fn(CL<> cl)
{
  return _float<H, L, (N - E)>{};
}

// decimal point
// high / low
template<int H, int L, int E, int I, int N, template<char, char...> class CL, char C, char... Cs>
auto constexpr make_float_fn(CL<C, Cs...> cl)
{
  if constexpr (C == '.')
  {
    auto constexpr _E = I + 1;
    return make_float_fn<H, L, _E, (I+1), N>(CL<Cs...>{});
  }
  else
  {
    auto constexpr _D = (C >= '0' && C <= '9');
    auto constexpr _H = (_D && E == 0)? (10 * H) + (C - '0'): H;
    auto constexpr _L = (_D && E > 0)?  (10 * L) + (C - '0'): L;
    return make_float_fn<_H, L, E, (I+1), N>(CL<Cs...>{});
  }
}


template<char... Cs>
auto constexpr operator""_f()
{
  // return make_float_t<0, 0, 0, 0, sizeof...(Cs), mp_chars<Cs...> >{};
  return make_float_fn<0, 0, 0, 0, sizeof...(Cs)>(mp_chars<Cs...>{});
}


template<auto H, auto L, auto E>
auto constexpr operator-(_float<H, L, E>)
{
  return _float<(-H), (-L), E>{};
}


template<auto H, auto L, auto E>
auto constexpr operator+(_float<H, L, E>)
{
  return _float<H, L, E>{};
}


int main()
{
  auto constexpr v = -4.2_f;
  float constexpr w = 2 * v;
  std::cout << w << std::endl;
}
