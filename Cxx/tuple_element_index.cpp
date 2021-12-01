namespace TupleHelper
{
  namespace detail
  {
    template<typename T, typename Tuple>
    struct tuple_element_index;

    template<typename T, template<class...> typename Tuple, typename... Rest>
    struct tuple_element_index<T, Tuple<T, Rest...>>
    {
      static auto constexpr value = std::size_t{0};
    };

    template<typename T, template<class...> typename Tuple, typename First, typename... Rest>
    struct tuple_element_index<T, Tuple<First, Rest...>>
    {
      static auto constexpr value = 1 + tuple_element_index<T, Tuple<Rest...>>::value;
    };
  }

  template<typename T, typename Tuple>
  struct tuple_element_index
  {
    static auto constexpr value = detail::tuple_element_index<T, Tuple>::value;
  };
}
