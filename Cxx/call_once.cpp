#include <iostream>

struct S {
  S() {
    static auto once = []{ std::cout << "once" << std::endl; return 0; }();
  }
};

int main() {
  S s1;
  S s2;
}
