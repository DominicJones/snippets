namespace detail
{

template <class T, std::size_t N, class... Args>
struct get_number_of_element_from_tuple_by_type_impl
{
    static constexpr auto value = N;
};

template <class T, std::size_t N, class... Args>
struct get_number_of_element_from_tuple_by_type_impl<T, N, T, Args...>
{
    static constexpr auto value = N;
};

template <class T, std::size_t N, class U, class... Args>
struct get_number_of_element_from_tuple_by_type_impl<T, N, U, Args...>
{
    static constexpr auto value = get_number_of_element_from_tuple_by_type_impl<T, N + 1, Args...>::value;
};

} // namespace detail

template <class T, class... Args>
T get_element_by_type(const std::tuple<Args...>& t)
{
    return std::get<detail::get_number_of_element_from_tuple_by_type_impl<T, 0, Args...>::value>(t);
}

int main()
{
    int a = 42;

    auto t = std::make_tuple(3.14, "Hey!", std::ref(a));

    get_element_by_type<int&>(t) = 43;

    std::cout << a << std::endl;

    // get_element_by_type<char>(t); // tuple_element index out of range

    return 0;
}
