#include <iostream>


#define Loop_begin(index, size)                      \
  for (auto index = 0; index != size; ++index)       \
  {                                                  \

#define Loop_end()                                   \
  }                                                  \


#define LambdaLoop_begin(index, size)                             \
  {                                                               \
    auto loopSize = size;                                         \
    auto loopBody = [&](auto index) mutable -> void               \
    {                                                             \

#define LambdaLoop_end()                                          \
    };                                                            \
    for (auto loopIndex = 0; loopIndex != loopSize; ++loopIndex)  \
    {                                                             \
      loopBody(loopIndex);                                        \
    }                                                             \
  }                                                               \


int main()
{
  int size = 5;
  int summ = 0;

  Loop_begin(it, size)
  {
    summ += it;
  }
  Loop_end();
  std::cout << summ << std::endl;

  LambdaLoop_begin(it, size)
  {
    summ += it;
  }
  LambdaLoop_end();
  std::cout << summ << std::endl;
}
