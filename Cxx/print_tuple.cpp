#include <iostream>
#include <tuple>

template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple(std::basic_ostream<Ch,Tr>& os, Tuple const& t, std::index_sequence<Is...>)
{
  using swallow = int[];
  (void)swallow{0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...};
}

template<class Ch, class Tr, class... Args>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t)
  -> std::basic_ostream<Ch, Tr>&
{
  os << "(";
  print_tuple(os, t, std::make_index_sequence<sizeof...(Args)>());
  return os << ")";
}

int main()
{
  std::cout << std::make_tuple(5, "Hello", -0.1) << std::endl;
}
