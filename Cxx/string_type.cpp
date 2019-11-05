// https://blog.galowicz.de/2016/05/14/converting_between_c_strings_and_type_lists/

#include <iostream>
#include <type_traits>
#include <utility>
#include <typeinfo>

// #include "demangle.h"


template <typename ... Types> struct mp_list {};

struct mp_void {};


template <char val>
struct mp_char
{
  static const constexpr char value {val};
};


template <class Str, std::size_t Pos, char C>
struct string_list;


template <class Str, std::size_t Pos, char C>
struct string_list
{
  using next_piece = typename string_list<
    Str,
    Pos + 1,
    Str::str()[Pos + 1]
    >::type;
  using type = mp_list<mp_char<C>, next_piece>;
};


template <class Str, std::size_t Pos>
struct string_list<Str, Pos, '\0'>
{
  using type = mp_void;
};


template <class Str>
using string_list_t = typename string_list<
  Str,
  0,
  Str::str()[0]
  >::type;


template <typename typelist, char ... chars>
struct flatten;

template <char c, typename restlist, char ... chars>
struct flatten<mp_list<mp_char<c>, restlist>, chars...>
  : public flatten<restlist, chars..., c>
{ };

template <char ... chars>
struct flatten<mp_void, chars...>
{
  static const char * const str()
  {
    static constexpr char ret[] {chars..., '\0'};
    return ret;
  }
};


struct FILE_macro
{
  static constexpr const char * str()
  {
    return __FILE__;
  }
};


int main()
{
  using file = string_list_t<FILE_macro>;
  std::cout << flatten<file>::str() << std::endl;
  // std::cout << demangle(file{}) << std::endl;
}
