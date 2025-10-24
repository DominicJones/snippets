// https://isocpp.org/files/papers/P2996R4.html
// https://www.modernescpp.com/index.php/reflection-in-c26-metafunctions/
// https://godbolt.org/z/fb35x4hYs

// clang -freflection-latest -std=c++26

#include <meta>
#include <iostream>
#include <string>




template<typename T>
auto consteval reflect_type() { return ^^T; }

template<typename T>
auto consteval reflect_type_member(int i) {
  if (i == 0) return ^^T::m0;
  if (i == 1) return ^^T::m1;
  return std::meta::info{};
}

struct S { int m0; char m1; };




template<typename E> 
struct reflect_enum_member_t
{ 
  std::string_view name; 
  E value;
};

template<typename E> 
auto consteval reflect_enum() 
{
  std::array<reflect_enum_member_t<E>, std::meta::enumerators_of(^^E).size()> result;
  int i = 0;

  for (auto member: std::meta::enumerators_of(^^E))
    result[i++] = reflect_enum_member_t<E>{std::meta::identifier_of(member), std::meta::extract<E>(member)};

  return result;
}

enum class Mode { disabled, enabled };




struct Vector { int x, y, z; };

template<typename T>
auto consteval reflect_has_member(std::string_view name)
{
  for (auto member: std::meta::members_of(^^T, std::meta::access_context::current()))
    if (std::meta::has_identifier(member) && std::meta::identifier_of(member) == name)
      return member;

  return std::meta::info{};
}




int main()
{
  typename[:^^int:] i = 3;

  auto constexpr mi_int = ^^int;
  typename[:mi_int:] j = 4;

  typename[:reflect_type<int>():] k = 5;

  S s;
  s.[:reflect_type_member<S>(0):] = 'c';
  s.[:reflect_type_member<S>(1):] = 3;


  static_assert(std::meta::identifier_of(^^Mode) == "Mode");
  static_assert(reflect_enum<Mode>()[0].name == "disabled");
  static_assert(reflect_enum<Mode>()[0].value == Mode::disabled);

  auto constexpr loc_t = std::meta::source_location_of(^^Mode);
  std::cout<<loc_t.line()<<std::endl;

  auto constexpr loc_v = std::meta::source_location_of(^^i);
  std::cout<<loc_v.line()<<std::endl;


  Vector v;
  v.[:reflect_has_member<Vector>("x"):] = 3;
  std::cout<<v.x<<std::endl;
}
