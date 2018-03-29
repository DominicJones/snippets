#include <type_traits>
#include <utility>


// C++17
namespace std
{
  template<typename...> struct make_void { using type = void; };
  template<typename... T> using void_t = typename make_void<T...>::type;
}


template<typename, typename = void>
struct HasValueType : std::false_type {};

template<typename T>
struct HasValueType<T, std::void_t<typename T::ValueType> > : std::true_type {};


template<typename T, bool>
struct ValueType { using Type = int; };

template<typename T>
struct ValueType<T, true> { using Type = typename T::ValueType; };


template <typename T>
struct MakeValueType
{
  using Type = typename ValueType<T, HasValueType<T>::value>::Type;
};


class Foo
{
public:
  using ValueType = float;
};

class Bar {};


int main()
{
  static_assert(std::is_same<MakeValueType<Foo>::Type, float>::value, "");
  static_assert(std::is_same<MakeValueType<Bar>::Type, int>::value, "");
}
