enum class Mode { pri, tng, adj, };

enum class DualSweepOption { intermediate_op, result_op, clear_op, };

template<typename F_PRI, typename F_DRV>
struct DualSweep
{
  DualSweep(Mode const &mode,
            F_PRI const &f_pri,
            F_DRV const &f_drv,
            DualSweepOption const &option)
    : _mode(mode)
    , _f_pri(f_pri)
    , _f_drv(f_drv)
    , _option(option)
  {
    if (_mode == Mode::pri)
    {
      if (_option != DualSweepOption::clear_op) _f_pri();
    }

    if (_mode == Mode::tng)
    {
      if (_option == DualSweepOption::intermediate_op) _f_pri();
      if (_option != DualSweepOption::clear_op) _f_drv();
    }

    if (_mode == Mode::adj)
    {
      if (_option == DualSweepOption::intermediate_op) _f_pri();
      if (_option == DualSweepOption::clear_op) _f_drv();
    }
  }

  ~DualSweep()
  {
    if (_mode == Mode::adj)
    {
      if (_option != DualSweepOption::clear_op) _f_drv();
    }

    if (_mode == Mode::tng)
    {
      if (_option == DualSweepOption::clear_op) _f_drv();
    }

    if (_mode == Mode::pri)
    {
      if (_option == DualSweepOption::clear_op) _f_pri();
    }
  }

  Mode const _mode;
  F_PRI const _f_pri;
  F_DRV const _f_drv;
  DualSweepOption const _option;
};


#include <iostream>


void eval(Mode mode)
{
  auto step1 = DualSweep{mode,
                         [](){std::cout << "*** step1 pri" << std::endl;},
                         [](){std::cout << "*** step1 drv" << std::endl;},
                         DualSweepOption::intermediate_op};

  auto step2 = DualSweep{mode,
                         [](){std::cout << "*** step2 pri" << std::endl;},
                         [](){std::cout << "*** step2 drv" << std::endl;},
                         DualSweepOption::result_op};

  auto step3 = DualSweep{mode,
                         [](){std::cout << "*** step3 pri" << std::endl;},
                         [](){std::cout << "*** step3 drv" << std::endl;},
                         DualSweepOption::clear_op};
}


int main()
{
  eval(Mode::pri);
  std::cout << std::endl;

  eval(Mode::tng);
  std::cout << std::endl;

  eval(Mode::adj);
}
