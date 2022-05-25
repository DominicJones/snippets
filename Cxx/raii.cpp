#include <iostream>

enum class Mode { primal, adjoint };


template<auto f>
struct RAII_F
{
  RAII_F() { f(Mode::primal); }
  ~RAII_F() { f(Mode::adjoint); }
};


template<typename C>
struct RAII_C
{
  RAII_C() : _cc(Mode::primal), _cd(Mode::adjoint) { _cc(); }
  ~RAII_C() { _cd(); }
  C const _cc;
  C const _cd;
};


void evaluate(Mode mode)
{
  std::cout << int(mode) << std::endl;
}


struct Evaluate
{
  Evaluate(Mode mode) : _mode(mode) {}
  void operator()() const
  {
    std::cout << int(_mode) << std::endl;
  }
  Mode const _mode;
};


int main()
{
  auto obj_f = RAII_F<evaluate>{};
  auto obj_c = RAII_C<Evaluate>{};
}
