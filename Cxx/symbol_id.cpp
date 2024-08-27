template<typename>
struct symbol_id
{
  static auto constexpr singleton = []{};
  static auto constexpr void = std::addressof(singleton);
};

template<auto ID = symbol_id<decltype([]{})>{}>
struct symbol
{
  static auto constexpr id = ID;
};

template<typename Lhs, typename Rhs>
bool constexpr operator<(symbol_id<Lhs>, symbol<Rhs>)
{
//  return symbol_id<Lhs>::address < symbol_id<Rhs>::address;
  return std::less{}(symbol_id<Lhs>::address, symbol_id<Rhs>::address);
}

template<typename Lhs, typename Rhs>
std::strong_ordering constexpr operator<=>(symbol_id<Lhs>, symbol<Rhs>)
{
  return std::compare_three_way{}(symbol_id<Lhs>::address, symbol_id<Rhs>::address);
}
