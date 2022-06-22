#include <type_traits>
#include <utility>


template<typename, typename = void>
struct has_member : std::false_type {};

template<typename T>
struct has_member<T, std::void_t<decltype(&T::member)> > : std::true_type {};

struct Pass
{
  void member(int const &) {}
};

struct Fail
{
  template<typename T> void member(T const &) {}
};


int main()
{
  static_assert(has_member<Pass>::value);
  static_assert(!has_member<Fail>::value);
}
