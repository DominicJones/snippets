#include <iostream>
#include <exception>
#include <stdexcept>
#include <limits>


template<class T>
class DivideByZero
 : public std::domain_error
{
public:
  DivideByZero(T const &op0, T const &op1)
    : std::domain_error("Divide by zero error")
    , _op0(op0)
    , _op1(op1)
  {}

  const char * what() const throw()
  {
    std::cout << "op0 = " << _op0 << ", op1 = " << _op1 << std::endl;
    std::domain_error::what();
  }

private:
  T const _op0;
  T const _op1;
};


double opDivide(double const &op0, double const &op1)
{
  if (op1 == double(0))
    throw DivideByZero<double>(op0, op1);

  return op0 / op1;
}


int main()
{
  double op0(1);
  double op1(0);
  double result(0);

  try
  {
    result = opDivide(op0, op1);
  }
  catch(DivideByZero<double> const &e)
  {
    std::cout << e.what() << std::endl;
    op1 = std::numeric_limits<double>::epsilon();
    result = opDivide(op0, op1);
  }

  std::cout << "result = " << result << std::endl;
}
