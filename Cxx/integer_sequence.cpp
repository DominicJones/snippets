#include <iostream>
#include <tuple>
#include <utility>


template<typename Func, typename Tup, std::size_t... index>
typename std::enable_if<bool(std::index_sequence<index...>::size() == 0)>::type
invoke_helper(Func&& func, Tup&& tup, std::index_sequence<index...>) {}


template<typename Func, typename Tup, std::size_t... index>
typename std::enable_if<bool(std::index_sequence<index...>::size() > 0)>::type
invoke_helper(Func&& func, Tup&& tup, std::index_sequence<index...>)
{
  constexpr auto IndexSize =
    std::index_sequence<index...>::size();

  invoke_helper(std::forward<Func>(func),
                std::forward<Tup>(tup),
                std::make_index_sequence<IndexSize - 1>{});

  func(std::get<IndexSize - 1>(tup));
}


template<typename Func, typename Tup>
void invoke(Func&& func, Tup&& tup)
{
  constexpr auto TupleSize =
    std::tuple_size<typename std::decay<Tup>::type>::value;

  invoke_helper(std::forward<Func>(func),
                std::forward<Tup>(tup),
                std::make_index_sequence<TupleSize>{});
}


void print(int const &arg)
{
  std::cout << arg << std::endl;
}


int main()
{
  {
    auto args = std::make_tuple(10);
    invoke(print, args);
  }

  {
    auto args = std::make_tuple(100, 200);
    invoke(print, args);
  }
}
