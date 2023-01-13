// C++20
#include <memory>
#include <cmath>


struct ct_string
{
  char _data[512]{};
  int _size{0};

  template<int N>
  constexpr ct_string(char const (&str)[N], int const M = (N-1))
    : _data{}
    , _size{std::max(N-1, M)}
  {
    for (int it = 0; it != (N-1); ++it) _data[it] = str[it];
    for (int it = (N-1); it != M; ++it) _data[it] = ' ';
  }
};


template<ct_string> struct print;


template<int N>
ct_string constexpr name_value(char const (&str)[N], int const value)
{
  int const n_pos = std::ceil(std::log10(value));
  ct_string result(str, N + n_pos);

  for (int it = 0; it != n_pos; ++it)
  {
    int const base = std::pow(10, n_pos - 1 - it);
    int const vc = (value % (10 * base)) / base;
    result._data[(N-1) + it] = vc;
  }

  return result;
}


int main()
{
  std::array<char, 4096> object;
  print<name_value("object size = ", sizeof(object))> _{};
}
