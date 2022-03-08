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


  template<typename... Ts> struct head;

  template<typename T, typename... Ts> struct head<type_sequence<T, Ts...>>
  {
    using type = T;
  };

  template<typename... Ts> using head_t = typename head<Ts...>::type;
 
 
  template<typename... Ts> struct tail;

  template<typename T, typename... Ts> struct tail<type_sequence<T, Ts...>>
  {
    using type = type_sequence<Ts...>;
  };

  template<typename... Ts> using tail_t = typename tail<Ts...>::type;


  template<typename T, typename L> struct cons;

  template<typename T, typename... Ts> struct cons<T, type_sequence<Ts...>>
  {
    using type = type_sequence<T, Ts...>;
  };

  template<typename T, typename L> using cons_t = typename cons<T, L>::type;
}
