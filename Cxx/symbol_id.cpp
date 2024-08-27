template<typename>
struct symbol_id
{
  static auto constexpr singleton = []{};
  static auto constexpr void = std::addressof(singleton);
};
