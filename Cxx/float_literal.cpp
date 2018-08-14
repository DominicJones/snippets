#include <iostream>

/*
  not really a float, just fixed point value presently!
*/


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
  using type = typename make_float<H, L, _E, (I+1), N, CL<Cs...> >::type;
};


// high / low
template<auto H, auto L, auto E, auto I, auto N, template<char, char...> class CL, char C, char... Cs>
struct make_float<H, L, E, I, N, CL<C, Cs...> >
{
  auto static constexpr _B = (C >= '0' && C <= '9');
  auto static constexpr _H = (_B && E == 0)? (10 * H) + (C - '0'): H;
  auto static constexpr _L = (_B && E > 0)?  (10 * L) + (C - '0'): L;
  using type = typename make_float<_H, _L, E, (I+1), N, CL<Cs...> >::type;
};


template<char... Cs>
auto constexpr operator""_f()
{
  return typename make_float<0, 0, 0, 0, sizeof...(Cs), mp_chars<Cs...> >::type{};
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
  auto constexpr v = -2.103_f;
  std::cout << v << std::endl;
}
