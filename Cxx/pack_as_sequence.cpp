namespace nonstd
{
  template<typename... Ts> struct type_sequence
  {
    static constexpr std::size_t size() noexcept { return sizeof...(Ts); }
  };

  template<typename T, T... vs> struct value_sequence
  {
    static constexpr std::size_t size() noexcept { return sizeof...(vs); }
  };
}
