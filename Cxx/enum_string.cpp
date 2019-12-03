#define DrvModeOptions(apply)                       \
  apply(Primal)                                     \
  apply(Tangent)                                    \
  apply(Adjoint)

#define TokenComma(token) token,
enum class DrvMode { DrvModeOptions(TokenComma) };
#undef TokenComma

constexpr const char* toString(DrvMode options) noexcept
{
#define TokenCaseSelect(token) case(DrvMode::token): return #token;
  switch(options) { DrvModeOptions(TokenCaseSelect) }
#undef TokenCaseSelect
}
#undef DrvModeOptions


#include <iostream>

int main()
{
  std::cout << toString(DrvMode::Primal) << std::endl;
  std::cout << toString(DrvMode::Tangent) << std::endl;
  std::cout << toString(DrvMode::Adjoint) << std::endl;
}
