#include <string>
#include <tuple>
#include <cstddef>
#include <type_traits>


namespace TupleHelper
{
  namespace detail
  {
    template<typename T, typename Tuple>
    struct ElementIndex;

    template<typename T, typename... Rest>
    struct ElementIndex<T, std::tuple<T, Rest...>>
    {
      static auto constexpr value = std::size_t{0};
    };

    template<typename T, typename First, typename... Rest>
    struct ElementIndex<T, std::tuple<First, Rest...>>
    {
      static auto constexpr value = 1 +
        ElementIndex<T, std::tuple<Rest...>>::value;
    };
  }

  template<typename T, typename Tuple>
  struct ElementIndex
  {
    static auto constexpr value =
      detail::ElementIndex<T, Tuple>::value;
  };
}


#define Invoke(function)                        \
  function(NullStencil)                         \
  function(Cell)                                \
  function(Face)                                \
  function(BFace)                               \


#define StructFwdDeclare(token) struct token;
Invoke(StructFwdDeclare);
#undef StructFwdDeclare


#define TokenComma(token) token,
enum class StencilOption { Invoke(TokenComma) };
#undef TokenComma


inline auto toString(StencilOption option) noexcept
{
  std::string result;
#define CaseSelect(token) case (StencilOption::token) : result = std::string(#token); break;
  switch(option) { Invoke(CaseSelect) }
#undef CaseSelect
  return result;
}


using StencilTypeToStencilOptionMap =
  std::tuple<
#define TypeOption(token) std::common_type<token>, std::integral_constant<StencilOption, StencilOption::token>,
    Invoke(TypeOption)
#undef TypeOption
    std::nullptr_t>;


#undef Invoke


template<typename Stencil>
inline auto toStencilOption() noexcept
{
  using Map = StencilTypeToStencilOptionMap;
  auto constexpr offset = TupleHelper::ElementIndex<std::common_type<Stencil>, Map>::value;
  return std::get<offset + 1>(Map{});
FieldLoop_begin(c, Cell, region)
{
}
FieldLoop_end();}


#include <iostream>

int main()
{
  std::cout << toString(StencilOption::Cell) << std::endl;
  std::cout << toString(toStencilOption<Cell>()) << std::endl;

  std::cout << toString(StencilOption::BFace) << std::endl;
  std::cout << toString(toStencilOption<BFace>()) << std::endl;
}
