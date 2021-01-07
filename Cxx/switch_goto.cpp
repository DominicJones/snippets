#include <iostream>
#include <utility>
#include <cassert>


struct DrvMode { enum Option : int { PRIMAL = 1, TANGENT, ADJOINT, }; };


#define DrvBlockSequence_init(mode, size, block)        \
  auto block{(mode == DrvMode::ADJOINT) ?               \
      std::make_pair((size - 1), size) :                \
      std::make_pair(0, size)};


#define DrvBlockSequence_begin(mode, start, block)      \
  start: switch (block.first) {                         \
    case 0:                                             \
    {


#define DrvBlockSequence_next(mode, start, block, index)        \
    }                                                           \
    (mode == DrvMode::ADJOINT) ? block.first-- : block.first++; \
    goto start;                                                 \
  case index:                                                   \
    {


#define DrvBlockSequence_end(mode, start, block)                \
    }                                                           \
    (mode == DrvMode::ADJOINT) ? block.first-- : block.first++; \
    goto start;                                                 \
  default:                                                      \
    if (mode == DrvMode::ADJOINT)                               \
      assert(block.first == -1);                                \
    else                                                        \
      assert(block.first == block.second);                      \
    break;                                                      \
  }



void eval(DrvMode::Option mode)
{
  DrvBlockSequence_init(mode, 4, block);

  DrvBlockSequence_begin(mode, start, block)
  {
    std::cout << "block " << block.first << std::endl;
  }
  DrvBlockSequence_next(mode, start, block, 1)
  {
    std::cout << "block " << block.first << std::endl;
  }
  DrvBlockSequence_next(mode, start, block, 2)
  {
    std::cout << "block " << block.first << std::endl;
  }
  DrvBlockSequence_next(mode, start, block, 3)
  {
    std::cout << "block " << block.first << std::endl;
  }
  DrvBlockSequence_end(mode, start, block);
}


int main()
{
  eval(DrvMode::TANGENT);
  eval(DrvMode::ADJOINT);
}
