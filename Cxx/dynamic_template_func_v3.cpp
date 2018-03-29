#include <array>
#include <utility>
#include <iostream>


template<int value>
int eval()
{
  return value;
}


// Cannot forward the name 'eval', so specific maker must be written. However,
// 'eval' could be wrapped a non-templated static method within a template struct
// and that struct be passed to a generic maker.
template<std::size_t... index>
constexpr
std::array<typename std::add_pointer<decltype(eval<0>)>::type, sizeof...(index)>
make_eval_list(std::index_sequence<index...>)
{
  return { eval<index>... };
}


int main()
{
  constexpr auto eval_fn =
    make_eval_list(std::make_index_sequence<3>{});

  for (int it = 0; it != eval_fn.size(); ++it)
    std::cout << eval_fn[it]() << std::endl;
}
