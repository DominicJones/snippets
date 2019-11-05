// https://blog.galowicz.de/2016/05/08/compile_time_type_lists/
// and https://github.com/tfc/cpp_template_meta_brainfuck_interpreter/blob/master/typelist.hpp

// #include "demangle.h"

struct null_t {};

template <typename T, typename U>
struct nested_list
{
  using head = T;
  using tail = U;
};


template <typename L, typename T>
using nested_prepend_t = nested_list<T, L>;


template <typename L, typename T>
struct variadic_prepend;

template <typename ... ListItems, typename T>
struct variadic_prepend<T, nested_list<ListItems...> >
{
  using type = nested_list<T, ListItems...>;
};

template <typename L, typename T>
using variadic_prepend_t = typename variadic_prepend<T, L>::type;



template <typename ... Ts> struct make_nested_list;

template <typename T, typename ... Ts>
struct make_nested_list<T, Ts...> {
    using type = nested_list<T, typename make_nested_list<Ts...>::type>;
};

template <>
struct make_nested_list<> { using type = null_t; };

template <typename ... Ts>
using make_nested_list_t = typename make_nested_list<Ts...>::type;


template <typename TList>
struct nested_list_content;

template <typename Head, typename Tail>
struct nested_list_content<nested_list<Head, Tail> >
{
  using head = Head;
  using tail = Tail;
};

template <>
struct nested_list_content<null_t>
{
  using head = null_t;
  using tail = null_t;
};

template <typename TList>
using head_t = typename nested_list_content<TList>::head;

template <typename TList>
using tail_t = typename nested_list_content<TList>::tail;




template <typename TList, typename T>
struct nested_append;

template <>
struct nested_append<null_t, null_t> { using type = null_t; };

template <typename T>
struct nested_append<null_t, T> { using type = nested_list<T, null_t>; };

template <typename Head, typename Tail, typename T>
struct nested_append<nested_list<Head, Tail>, T>
{ using type = nested_list<Head, typename nested_append<Tail, T>::type>; };

template <typename TList, typename T>
using nested_append_t = typename nested_append<TList, T>::type;


template <typename T> class debug_t;


int main()
{
  using list1 = nested_list<bool,
                            nested_list<char,
                                        nested_list<int, null_t> > >;
  using list2 = nested_prepend_t<list1, float>;
  using list3 = make_nested_list_t<bool, char, int>;
  using list4 = nested_append_t<list3, float>;

  // std::cout << demangle(list4{}) << std::endl;
  debug_t<list4> d;
}
