// Avoid having a set of types with corresponding enumerations

#include <iostream>
#include <type_traits>


class PrimalMode;
class TangentMode;
class AdjointMode;


class DrvMode
{
private:
  template<class DrvMode_t> class Overload {};

public:
  virtual ~DrvMode() {}

  template<class DrvMode_t> bool static is(DrvMode const &mode)
  {
    return is(Overload<DrvMode_t>(), mode);
  }

private:
  template<class DrvMode_t>
  bool static is(Overload<DrvMode_t>, DrvMode const &mode);

  bool static is(Overload<PrimalMode>, DrvMode const &mode);
  bool static is(Overload<TangentMode>, DrvMode const &mode);
  bool static is(Overload<AdjointMode>, DrvMode const &mode);
};


class PrimalMode: public DrvMode {};
class TangentMode: public DrvMode {};
class AdjointMode: public DrvMode {};


bool
DrvMode::
is(Overload<PrimalMode>, DrvMode const &mode)
{
  return dynamic_cast<PrimalMode const * const>(&mode);
}

bool
DrvMode::
is(Overload<TangentMode>, DrvMode const &mode)
{
  return dynamic_cast<TangentMode const * const>(&mode);
}

bool
DrvMode::
is(Overload<AdjointMode>, DrvMode const &mode)
{
  return dynamic_cast<AdjointMode const * const>(&mode);
}


template<class DrvMode_t>
void staticEval()
{
  if (std::is_same<DrvMode_t, PrimalMode>::value)
    std::cout << "pri" << std::endl;

  if (std::is_same<DrvMode_t, TangentMode>::value)
    std::cout << "tng" << std::endl;

  if (std::is_same<DrvMode_t, AdjointMode>::value)
    std::cout << "adj" << std::endl;
}


void eval(DrvMode const &mode)
{
  if (DrvMode::is<PrimalMode>(mode))
    staticEval<PrimalMode>();

  if (DrvMode::is<TangentMode>(mode))
    staticEval<TangentMode>();

  if (DrvMode::is<AdjointMode>(mode))
    staticEval<AdjointMode>();
}


int main()
{
  eval(PrimalMode());
  eval(TangentMode());
  eval(AdjointMode());
}
