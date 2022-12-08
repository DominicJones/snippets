#include <iostream>

enum class Cast { Static, Dynamic, Reinterpret, C_Style, };

template<Cast c, typename T>
auto constexpr cast = [](auto &&v)
{
  if constexpr (c == Cast::Static) return static_cast<T>(v);
  if constexpr (c == Cast::Dynamic) return dynamic_cast<T>(v);
  if constexpr (c == Cast::Reinterpret) return reinterpret_cast<T>(v);
  /* (c == Cast::C_Style) */ return (T)v;
};

int main() {
  std::cout << cast<Cast::Static, int>(3.14) << std::endl;
}
