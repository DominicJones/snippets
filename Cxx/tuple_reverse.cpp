// from: http://stackoverflow.com/a/17178399


// 1
template <typename... Ts>
struct tuple_reverse;

template <>
struct tuple_reverse<std::tuple<>>
{
    using type = std::tuple<>;
};

template <typename T, typename... Ts>
struct tuple_reverse<std::tuple<T, Ts...>>
{
  using head = std::tuple<T>;
  using tail = typename tuple_reverse<std::tuple<Ts...>>::type;

  using type = decltype(std::tuple_cat(std::declval<tail>(), std::declval<head>()));
};


// 2
template<typename T, size_t... I>
struct tuple_reverse_impl<T, std::index_sequence<I...>>
{
  typedef std::tuple<typename std::tuple_element<sizeof...(I) - 1 - I, T>::type...> type;
};

// partial specialization for handling empty tuples:
template<typename T>
struct tuple_reverse_impl<T, std::index_sequence<>>
{
  typedef T type;
};

template<typename T>
struct tuple_reverse<T>
: tuple_reverse_impl<T, std::make_index_sequence<std::tuple_size<T>::value>>
{ };


// 3
template<typename T, size_t... I>
auto
reverse_impl(T&& t, std::index_sequence<I...>)
{
  return std::make_tuple(std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
}

template<typename T>
auto
reverse(T&& t)
{
  return reverse_impl(std::forward<T>(t),
                      std::make_index_sequence<std::tuple_size<T>::value>());
}


// 4
template<typename T, typename TT = typename std::remove_reference<T>::type, size_t... I>
auto
reverse_impl(T&& t, redi::index_sequence<I...>)
-> std::tuple<typename std::tuple_element<sizeof...(I) - 1 - I, TT>::type...>
{
    return std::make_tuple(std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
}

template<typename T, typename TT = typename std::remove_reference<T>::type>
auto
reverse(T&& t)
-> decltype(reverse_impl(std::forward<T>(t),
                        redi::make_index_sequence<std::tuple_size<TT>::value>()))
{
    return reverse_impl(std::forward<T>(t),
                        redi::make_index_sequence<std::tuple_size<TT>::value>());
}
