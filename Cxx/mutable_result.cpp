#include <iostream>
#include <memory>
#include <functional>


double a = 3;
double b = 4;

double a_tng = 0;
double b_tng = 0;

double a_adj = 0;
double b_adj = 0;


template<class T>
class MutableResult
{
public:
  using OpAddAssign = std::function<void (T const &result)>;
  using Type = T;

  MutableResult(T const &result)
    : _result(result)
    , _opAddAssign(nullptr)
  {}

  MutableResult(OpAddAssign const &opAddAssign)
    : _result(0)
    , _opAddAssign(opAddAssign)
  {}

  operator T const &() const { return _result; }

  void operator+=(T const &result) { if (_opAddAssign) _opAddAssign(result); }
  void operator-=(T const &result) { if (_opAddAssign) _opAddAssign(T(-result)); }

private:
  T const _result;
  OpAddAssign const _opAddAssign;
};


MutableResult<double> evaluatePrimal()
{
  return MutableResult<double>(a * b);
}

MutableResult<double> evaluateTangent()
{
  return MutableResult<double>(a * b_tng + a_tng * b);
}

MutableResult<double> evaluateAdjoint()
{
  auto opAddAssign = [&](double const &result)
    {
      a_adj += b * result;
      b_adj += a * result;
    };

  return MutableResult<double>(opAddAssign);
}


int main()
{
  {
    auto const c = evaluatePrimal();
    std::cout << c << std::endl;
  }

  {
    a_tng = 1;
    auto c = evaluateTangent();
    std::cout << c << std::endl;
  }

  {
    double c = 1;
    evaluateAdjoint() += c;
    std::cout << a_adj << std::endl;
  }
}
