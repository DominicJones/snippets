#include <iostream>

// A trait is a small object whose main purpose is to carry information used by
// another object or algorithm to determine "policy" or "implementation details"

template<bool b>
struct SelectEvaluate
{
  template<typename T>
  static void evaluate(T& object) {
    std::cout << "standard implementation" << std::endl;
  }
};

template<>
struct SelectEvaluate<true>
{
  template<typename T>
  static void evaluate(T& object) {
    object.evaluate();
  }
};


template<typename T>
struct IsOptimised
{
  static const bool value = false;
};

class System2;
template<>
struct IsOptimised<System2>
{
  static const bool value = true;
};


template<typename T>
void evaluate(T object)
{
  SelectEvaluate<IsOptimised<T>::value>::evaluate(object);
}


class System1
{};

class System2
{
public:
  void evaluate() {
    std::cout << "optimised implementation" << std::endl;
  }
};


int main(int argc, char* argv[]) {
  evaluate(System1());
  evaluate(System2());
}
