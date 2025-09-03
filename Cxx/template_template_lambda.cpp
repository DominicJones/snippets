#include <iostream>
#include <type_traits>

enum class Cast { Null, Static, Dynamic, Reinterpret, C_Style, };

template<typename T, Cast c = Cast::Static>
inline auto constexpr cast = []<typename V>(V &&v) noexcept
{
  if constexpr (std::is_same_v<T, std::decay_t<V>>) return v;
  else if constexpr (c == Cast::Static) return static_cast<T>(v);
  else if constexpr (c == Cast::Dynamic) return dynamic_cast<T>(v);
  else if constexpr (c == Cast::Reinterpret) return reinterpret_cast<T>(v);
  else if constexpr (c == Cast::C_Style) return (T)v;
  else static_assert(false, "invalid cast");
};

int main()
{
  double v = 3.14;
  std::cout << cast<float>(v) << std::endl;
}
