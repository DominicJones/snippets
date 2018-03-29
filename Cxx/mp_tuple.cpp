#include <iostream>


// mp_tuple
template<std::size_t idx, typename... Ts>
struct mp_tuple_impl;

template<std::size_t idx>
struct mp_tuple_impl<idx>
{};

template<std::size_t idx, typename T1, typename... Ts>
struct mp_tuple_impl<idx, T1, Ts...>
  : mp_tuple_impl<idx + 1, Ts...>
{
  using base_t = mp_tuple_impl<idx + 1, Ts...>;
  mp_tuple_impl() : base_t() {}
  mp_tuple_impl(T1 value, Ts... values) : value(value), base_t(values...) {}
  T1 value;
};

template<typename... Ts>
struct mp_tuple
  : mp_tuple_impl<0, Ts...>
{
  using base_t = mp_tuple_impl<0, Ts...>;
  mp_tuple() : base_t() {}
  mp_tuple(Ts... values) : base_t(values...) {}
};

template<>
struct mp_tuple<>
{
  mp_tuple() {}
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
T1 mp_tuple_get_impl(mp_tuple_impl<idx, T1, Ts...> const &t)
{
  return t.value;
}

template<std::size_t idx, typename... Ts>
typename mp_tuple_element<idx, mp_tuple<Ts...> >::type
mp_tuple_get(mp_tuple<Ts...> const &t)
{
  return mp_tuple_get_impl<idx>(t);
}


int main()
{
  mp_tuple<int, int, double> t{3, 4, 5.6};
  mp_tuple<> u;
  std::cout << mp_tuple_get<1>(t) << std::endl;
}
