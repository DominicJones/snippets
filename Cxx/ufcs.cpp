#include <utility>

#define CPP2_UFCS(FUNCNAME, PARAM1, ...)                                \
  [](auto &&obj, auto &&...params) {                                    \
    if constexpr (requires{ std::forward<decltype(obj)>(obj).FUNCNAME(std::forward<decltype(params)>(params)...); }) { \
      return std::forward<decltype(obj)>(obj).FUNCNAME(std::forward<decltype(params)>(params)...); \
    } else {                                                            \
      return FUNCNAME(std::forward<decltype(obj)>(obj), std::forward<decltype(params)>(params)...); \
    }                                                                   \
  }(PARAM1, __VA_ARGS__)

#define CPP2_UFCS_0(FUNCNAME, PARAM1)                                   \
  [](auto &&obj) {                                                      \
    if constexpr (requires{ std::forward<decltype(obj)>(obj).FUNCNAME(); }) { \
      return std::forward<decltype(obj)>(obj).FUNCNAME();               \
    } else {                                                            \
      return FUNCNAME(std::forward<decltype(obj)>(obj));                \
    }                                                                   \
  }(PARAM1)


#include <iostream>

struct Vector {
  void unit() {}
};

struct Tensor {
};
void transpose(Tensor const &) {}


int main()
{
  Vector vector;
  CPP2_UFCS_0(unit, vector);

  Tensor tensor;
  CPP2_UFCS_0(transpose, tensor);
}
