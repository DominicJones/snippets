#include <array>

template<typename T, typename U>
struct pair
{
  T first;
  U second;
};

template<typename T, typename U>
pair(T, U) -> pair<T, U>;

template<pair p>
constexpr auto use()
{
  std::array<int, p.first> data{p.second};
  return data;
}

int main()
{
  constexpr pair values{1, 2};
  use<values>();
}
