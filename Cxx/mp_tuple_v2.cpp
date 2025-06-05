/*
  Inspired by:
  https://stackoverflow.com/a/77764774
  https://mcyoung.xyz/2022/07/13/tuples-the-hard-way/
*/

#include<print>
#include<string>

namespace detail
{
  static constexpr auto makeTuple(auto && ...args)
  {
    return [...args = std::forward<decltype(args)>(args)]<std::size_t I>() -> auto &&
    {
      return args...[I];
    };
  }
}

template<typename ...T>
struct Tuple
{
  Tuple(auto && ...args)
    : data(detail::makeTuple(std::forward<decltype(args)>(args)...))
  {}

  decltype(detail::makeTuple(std::declval<T>()...)) data;
};

Tuple(auto && ...args) -> Tuple<decltype(args)...>;

int main()
{ 
  using namespace std::literals;
  auto const tuple = Tuple(42, 4.2, 'c', "msg"s);
  std::println("tuple[3] = {}", tuple.data.template operator()<3>());
}
