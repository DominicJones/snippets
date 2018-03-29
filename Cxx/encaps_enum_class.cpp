#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include <bitset>
#include <climits>
#include <cstdint>


class Mode
{
private:
  enum class Option: uint16_t
  {
    PRIMAL = 0x0001,
    TANGENT = 0x0002,
    ADJOINT = 0x0004,
    IS_ADJOINT_CONTEXT = 0x0100,
  };

  using Integral_t = std::underlying_type<Option>::type;
  static constexpr std::size_t integral_s = sizeof(Integral_t) * CHAR_BIT;

public:
  static Mode primal()  { return Mode(Mode::Option::PRIMAL); }
  static Mode tangent() { return Mode(Mode::Option::TANGENT); }
  static Mode adjoint() { return Mode(Mode::Option::ADJOINT); }

  Mode(Option const option)
    : _option(option)
  {}

  Mode option() const
  {
    return Mode(Option(Integral_t(_option) & 0xff));
  }

  Mode inAdjointContext() const
  {
    return Mode(Option(Integral_t(this->option()) |
                       Integral_t(Mode::Option::IS_ADJOINT_CONTEXT)));
  }

  bool isAdjointContext() const
  {
    return bool(Integral_t(_option) &
                Integral_t(Mode::Option::IS_ADJOINT_CONTEXT));
  }

  operator Integral_t() const
  {
    return Integral_t(_option);
  }

  std::bitset<integral_s> asBitset() const
  {
    return std::bitset<integral_s>(Integral_t(_option));
  }

private:
  Option const _option;
};


int main(int argc, char** argv)
{
  Mode mode(Mode::primal().inAdjointContext());

  if (mode.option() == mode.primal())
    std::cout << "is primal" << std::endl;

  if (mode.isAdjointContext())
    std::cout << "is adjoint context" << std::endl;

  std::cout << mode.asBitset() << std::endl;
  std::cout << mode.option().asBitset() << std::endl;
}
