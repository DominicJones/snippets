enum class direction : int { forward, reverse };


#define sequence_begin(mode)                    \
{                                               \
  auto constexpr sequence_offset{__COUNTER__};  \
  auto const sequence_mode{mode};               \
  auto sequence_case{int(0)};                   \
  if (mode == direction::reverse)               \
    goto sequence_end_label;                    \
sequence_begin_label:                           \
  switch (sequence_case) {                      \
  case 0:                                       \
  {


#define sequence_next()                         \
  }                                             \
  (sequence_mode == direction::reverse) ?       \
    sequence_case-- : sequence_case++;          \
  goto sequence_begin_label;                    \
  case (__COUNTER__ - sequence_offset):         \
  {


#define sequence_end()                                  \
  }                                                     \
  (sequence_mode == direction::reverse) ?               \
    sequence_case-- : sequence_case++;                  \
  goto sequence_begin_label;                            \
sequence_end_label:                                     \
  default:                                              \
  {                                                     \
    auto constexpr size{__COUNTER__ - sequence_offset}; \
    if (sequence_mode == direction::reverse &&          \
        sequence_case >= 0)                             \
    {                                                   \
      sequence_case = size - 1;                         \
      goto sequence_begin_label;                        \
    }                                                   \
  }                                                     \
  break;                                                \
  }                                                     \
}


#include <iostream>

int main()
{
  // auto d = direction::forward;
  auto d = direction::reverse;

  sequence_begin(d)
  {
    std::cout << "step 1" << std::endl;
  }
  sequence_next()
  {
    std::cout << "step 2" << std::endl;
  }
  sequence_next()
  {
    std::cout << "step 3" << std::endl;
  }
  sequence_end();
}
