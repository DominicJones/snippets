#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <typeinfo>
#include <vector>

#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <type_traits>


class Eval
{
private:
  enum class Release { no, yes };
  using Data = std::pair<std::unique_ptr<double const>, Release>;

public:
  Eval()
    : _data(Data::first_type(new Data::first_type::element_type(1)), Release::no)
  {}

  Eval(double const &value)
    : _data(Data::first_type(&value), Release::yes)
  {}

  Eval(double const &&value) = delete;

  ~Eval()
  {
    if (_data.second == Release::yes)
      _data.first.release();
  }

  Data::first_type::element_type value() const
  {
    return *_data.first;
  }

private:
  Data _data;
};


int main(int argc, char** argv)
{
  {
    std::cout << Eval().value() << std::endl;
  }

  {
    double const value(2);
    std::cout << Eval(value).value() << std::endl;
  }

  /* does not compile
  {
    std::cout << Eval(double(3)).value() << std::endl;
  }
  */
}
