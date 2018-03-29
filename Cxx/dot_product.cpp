#include <vector>

double dot_product(std::vector<double> a, std::vector<double> b)
{
  double result = 0;

  for (int i = 0; i != a.size(); ++i)
  {
    result += a[i] * b[i];
  }

  return result;
}
