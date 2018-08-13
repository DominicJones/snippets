template<auto V>
struct Literal
{
  auto constexpr static value = V;
  auto constexpr operator()() const { return V; }
  constexpr operator auto() const { return V; }
};


template<auto V>
auto constexpr make_literal()
{
  return V;
}


template<auto V, char C, char... Cs>
auto constexpr make_literal()
{
  auto constexpr V_dec = 10 * V;
  auto constexpr C_dec = C - '0';
  return make_literal<V_dec + C_dec, Cs...>();
}


template<char... Cs>
auto constexpr operator "" _l()
{
  return Literal<make_literal<0, Cs...>()>{};
}


int main()
{
  auto constexpr v = 42_l;
  static_assert(v == Literal<42>{});
}
