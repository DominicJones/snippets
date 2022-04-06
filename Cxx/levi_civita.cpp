#include <array>

class LeviCivita
{
private:
  template<typename T = short> using V3 = std::array<T, 3>;

public:
  static inline auto constexpr value(int i, int j, int k)
  {
    return ((i-j)*(j-k)*(k-i))/2;
  }

  static inline auto constexpr build()
  {
    V3<V3<V3<>>> data{0};
    for (int i = 0; i != 3; ++i)
      for (int j = 0; j != 3; ++j)
        for (int k = 0; k != 3; ++k)
          data[i][j][k] = value(i, j, k);
    return data;
  }

  inline constexpr LeviCivita() : _data(build()) {}

  inline auto constexpr operator()(short i, short j, short k) const
  {
    return _data[i][j][k];
  }

private:
  V3<V3<V3<>>> const _data;
};


#include <iostream>
#include <cassert>

int main()
{
  auto constexpr lcv{LeviCivita::build()};
  auto constexpr lc{LeviCivita{}};

  for (int i = 0; i != 3; ++i)
    for (int j = 0; j != 3; ++j)
      for (int k = 0; k != 3; ++k)
        assert(bool(lc(i,j,k) == lcv[i][j][k]));
}
