#include <iostream>

template<typename T>
auto constexpr c_cast = [](auto v){ return (T)v; };

int main() {
  std::cout << c_cast<int>(3.14) << std::endl;
}
