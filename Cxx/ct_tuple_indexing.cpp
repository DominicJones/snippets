/*
  From Template-less Metaprogramming - Cppcon-2024
  Presentation - https://www.youtube.com/watch?v=yriNqhv-oM0
  Slides - https://krzysztof-jusiak.github.io/talks/cppcon-2024
  Implemenation baesd on - https://github.com/qlibs/mp
*/

#include<array>
#include<print>


inline namespace detail
{
  template<auto> struct info
  {
    constexpr auto friend get(info);
  };

  template<class T> struct type
  {
    using value_type = T;
    static constexpr std::size_t id{};
    constexpr auto friend get(info<&id>) { return type{}; }
  };

  template<typename T>
  inline constexpr auto meta = &type<T>::id;
}


template<typename T, typename ...Ts>
auto consteval findIndex()
{
  auto constexpr types = std::array{meta<Ts>...};

  for (std::size_t it = 0; it != types.size(); ++it)
    if (types[it] == meta<T>) return it;

  return types.size();
}


template<auto I, typename ...T>
struct tuple_element
{
  using type = T...[I];
};

template<auto I, typename ...T>
using tuple_element_t = tuple_element<I, T...>::type;


static_assert(findIndex<int, float, char, int>() == 2);
static_assert(std::same_as<tuple_element_t<1, float, char, int>, char>);

int main(){}
