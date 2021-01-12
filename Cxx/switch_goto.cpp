struct DrvMode { enum Option : int { PRIMAL, TANGENT, ADJOINT, }; };


#define DrvSequence_begin(mode)                             \
  {                                                         \
    auto constexpr DrvSequence_offset{__COUNTER__};         \
    auto const DrvSequence_mode{mode};                      \
    auto DrvSequence_case{int(0)};                          \
    if (mode == DrvMode::ADJOINT)                           \
    {                                                       \
      goto DrvSequence_end_label;                           \
    }                                                       \
    DrvSequence_begin_label:                                \
    switch (DrvSequence_case) {                             \
    case 0:                                                 \
    {


#define DrvSequence_next()                                  \
    }                                                       \
    (DrvSequence_mode == DrvMode::ADJOINT) ?                \
      DrvSequence_case-- :                                  \
      DrvSequence_case++;                                   \
    goto DrvSequence_begin_label;                           \
    case (__COUNTER__ - DrvSequence_offset):                \
    {


#define DrvSequence_end()                                      \
    }                                                          \
    (DrvSequence_mode == DrvMode::ADJOINT) ?                   \
      DrvSequence_case-- :                                     \
      DrvSequence_case++;                                      \
    goto DrvSequence_begin_label;                              \
    DrvSequence_end_label:                                     \
    default:                                                   \
    {                                                          \
      auto constexpr size{__COUNTER__ - DrvSequence_offset};   \
      if (DrvSequence_mode == DrvMode::ADJOINT)                \
      {                                                        \
        if (DrvSequence_case >= 0)                             \
        {                                                      \
          DrvSequence_case = size - 1;                         \
          goto DrvSequence_begin_label;                        \
        }                                                      \
      }                                                        \
    }                                                          \
    break;                                                     \
    }                                                          \
  }


#include <iostream>


void eval(DrvMode::Option mode)
{
  DrvSequence_begin(mode)
  {
    std::cout << "0" << std::endl;
  }
  DrvSequence_next()
  {
    std::cout << "1" << std::endl;
  }
  DrvSequence_next()
  {
    std::cout << "2" << std::endl;
  }
  DrvSequence_next()
  {
    std::cout << "3" << std::endl;
  }
  DrvSequence_end();
}


int main()
{
  eval(DrvMode::TANGENT);
  eval(DrvMode::ADJOINT);
}
