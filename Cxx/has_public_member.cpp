#include <utility>


// C++17
namespace std
{
  template<typename...> struct make_void { using type = void; };
  template<typename... T> using void_t = typename make_void<T...>::type;
}


template<class, class = void>
struct has_public_data_member : std::false_type {};

template<class T>
struct has_public_data_member<T, std::void_t<decltype(T::member)> > : std::true_type {};


template<class, class = void>
struct has_public_function_member : std::false_type {};

template<class T>
struct has_public_function_member<T, std::void_t<decltype(&T::member)> > : std::true_type {};


struct ValidObject1 { int member; };
struct InvalidObject1 { int member_; };

struct ValidObject2 { int member() { return 0; }; };
struct InvalidObject2 { int member_() { return 0; }; };


int main()
{
  static_assert(has_public_data_member<ValidObject1>::value, "");
  static_assert(!has_public_data_member<InvalidObject1>::value, "");

  static_assert(has_public_function_member<ValidObject2>::value, "");
  static_assert(!has_public_function_member<InvalidObject2>::value, "");
}
