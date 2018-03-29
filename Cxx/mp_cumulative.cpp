// mp_tuple
template<std::size_t idx, typename... Ts>
struct mp_tuple_impl;

template<std::size_t idx>
struct mp_tuple_impl<idx>
{
  static std::size_t constexpr value = 0;
};

template<std::size_t idx, typename T1, typename... Ts>
struct mp_tuple_impl<idx, T1, Ts...>
  : mp_tuple_impl<idx + 1, Ts...>
{
  using base_t = mp_tuple_impl<idx + 1, Ts...>;
  static std::size_t constexpr value = base_t::value + T1::value;
};

template<typename... Ts>
struct mp_tuple
  : mp_tuple_impl<0, Ts...>
{
  using base_t = mp_tuple_impl<0, Ts...>;
  static std::size_t constexpr value = base_t::value;
};

template<>
struct mp_tuple<>
{
};


// mp_tuple_element
template<std::size_t idx, typename _Tp>
struct mp_tuple_element;

template<std::size_t idx, typename T1, typename... Ts>
struct mp_tuple_element<idx, mp_tuple<T1, Ts...> >
  : mp_tuple_element<idx - 1, mp_tuple<Ts...> >
{};

template<typename T1, typename... Ts>
struct mp_tuple_element<0, mp_tuple<T1, Ts...> >
{
  using type = T1;
};


// mp_tuple_get
template<std::size_t idx, typename T1, typename... Ts>
constexpr std::size_t mp_tuple_get_impl(mp_tuple_impl<idx, T1, Ts...>)
{
  return mp_tuple_impl<idx, T1, Ts...>::value;
}

template<std::size_t idx, typename... Ts>
constexpr std::size_t mp_tuple_get(mp_tuple<Ts...>)
{
  return mp_tuple_get_impl<idx>(mp_tuple<Ts...>{});
}


// mp_tuple_get_t
template<std::size_t idx, typename... Ts>
struct mp_tuple_get_t;

template<std::size_t idx, typename... Ts>
struct mp_tuple_get_t<idx, mp_tuple<Ts...> >
{
  static auto constexpr value = mp_tuple_get<idx>(mp_tuple<Ts...>{});
};
