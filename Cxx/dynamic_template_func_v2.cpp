#include <array>
#include <tuple>
#include <iostream>


template<int scale>
int eval(int value)
{
  return value * scale;
}


template<typename eval_t, std::size_t index_size, std::size_t... index>
constexpr
std::array<eval_t, index_size>
make_array(std::index_sequence<index...>)
{
  return std::array<eval_t, index_size>{ eval<index>... };
}


int main()
{
  using eval_t = std::add_pointer<int (int)>::type;
  constexpr std::size_t index_size = 4;

  constexpr std::array<eval_t, index_size> eval_fn =
    make_array<eval_t, index_size>(std::make_index_sequence<index_size>{});

  for (int it = 0; it != index_size; ++it)
    std::cout << eval_fn[it](it) << std::endl;
}
