#include <iostream>


class Status
{
public:
  enum Mode { Off = 1, On = 2, };

  Status(Mode const &refMode)
    : _refMode(refMode)
  {}

  Mode getRefMode() const { return _refMode; }

  // BUG: remove reference
  Mode const &_refMode;
};


class BitRep: public Status
{
public:
  BitRep() : Status(Status::On) {}
};


int main()
{
  std::cout << BitRep()._refMode << std::endl;
  std::cout << BitRep().getRefMode() << std::endl;
}
