#include <bitset>
#include <cassert>

enum Flags
{
 FlagNull = 0,
 FlagA = 1,
 FlagB = 2,
 FlagC = 4,
 FlagD = 8,
 FlagSize = 16,
};

auto to_bitset(int value) { return static_cast<std::bitset<8>>(value); }

int main()
{
  std::underlying_type_t<Flags> flag;

  // enable all flags
  flag = FlagSize - 1;
  std::cout << "*** " << to_bitset(flag) << std::endl;

  // disable only FlagA
  flag &= ~FlagA;
  std::cout << "*** " << to_bitset(flag) << std::endl;

  // disable all flags
  flag = FlagNull;
  std::cout << "*** " << to_bitset(flag) << std::endl;

  // enable FlagB
  flag |= FlagB;
  std::cout << "*** " << to_bitset(flag) << std::endl;

  // enable FlagC
  flag |= FlagC;
  std::cout << "*** " << to_bitset(flag) << std::endl;

  // check FlagB is enabled
  assert(flag & FlagB);

  // check FlagB or FlagC is enabled
  assert(flag & (FlagB | FlagC));

  // check FlagD is disabled
  assert((flag & FlagD) == FlagNull);
}
