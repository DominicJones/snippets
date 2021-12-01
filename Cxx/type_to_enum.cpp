#include <string>
#include <tuple>
#include <cstddef>
#include <type_traits>

#include "TupleHelper.h"


#define Colour_Invoke(function)                \
  function(NullColour)                         \
  function(Red)                                \
  function(Blue)                               \
  function(Green)                              \


#define Colour_ClassFwdDeclare(token) class token;
Colour_Invoke(Colour_ClassFwdDeclare);
#undef Colour_ClassFwdDeclare


#define Colour_TokenComma(token) token,
enum class ColourOption { Colour_Invoke(Colour_TokenComma) };
#undef Colour_TokenComma


inline auto toString(ColourOption option) noexcept
{
  std::string result;
#define Colour_CaseSelect(token) case (ColourOption::token) : { result = std::string(#token); break; }
  switch(option) { Colour_Invoke(Colour_CaseSelect) }
#undef Colour_CaseSelect
  return result;
}


using ColourTypeToColourOptionMap =
  std::tuple<
#define Colour_TypeOption(token) std::type_identity<token>, std::integral_constant<ColourOption, ColourOption::token>,
    Colour_Invoke(Colour_TypeOption)
#undef Colour_TypeOption
    std::nullptr_t>;


template<typename Colour>
auto toColourOption() noexcept
{
  using Map = ColourTypeToColourOptionMap;
  auto constexpr offset = TupleHelper::tuple_element_index<std::type_identity<Colour>, Map>::value;
  return std::get<offset + 1>(Map{});
}
