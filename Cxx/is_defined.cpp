#include <type_traits>

template<typename T, typename = void>
struct is_defined : std::false_type {};

template<typename T>
struct is_defined<T, decltype(void(sizeof(T)))> : std::true_type {};

struct Undefined;
struct Defined {};

int main()
{
  static_assert(!is_defined<Undefined>::value, "");
  static_assert(is_defined<Defined>::value, "");
}
