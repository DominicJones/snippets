#include <type_traits>


template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<(I == sizeof...(Tp))>::type
evaluate(std::tuple<Tp...> &) {}


template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<(I < sizeof...(Tp))>::type
evaluate(std::tuple<Tp...> &t)
{
  evaluate<I + 1, Tp...>(t);
}


int main()
{
  evaluate(std::tuple<char, int, float>{});
}
