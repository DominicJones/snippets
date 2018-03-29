// compile with: -std=c++11 <filename> -pthread

#include <iostream>
#include <thread>
#include <vector>

struct Evaluate0 {
  Evaluate0(float &c) : _c(c) {}
  void operator()(float const &a, float const &b) { _c = a * b; }
private:
  float &_c;
};

struct Evaluate1 {
  Evaluate1(float &c) : _c(c) {}
  void operator()(float const &a, float const &b) { _c = a + b; }
private:
  float &_c;
};

float blend(float const &w0, float const &c0, float const &c1) {
  return w0 * c0 + (1 - w0) * c1;
}

int main() {
  int const nThreads = 2;
  std::vector<std::thread> threads(nThreads);

  float const a = 3;
  float const b = 3;
  float c0;
  float c1;

  threads[0] = std::thread((Evaluate0(c0)), a, b);
  threads[1] = std::thread((Evaluate1(c1)), a, b);

  threads[0].join();
  threads[1].join();

  float c = blend(0.5, c0, c1);

  std::cout << "c: " << c << std::endl;
}
