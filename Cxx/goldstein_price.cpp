#include <iostream>
#include <algorithm>
#include <cmath>


void GoldsteinPrice(double x, double y, double *f)
{
  double t1 = pow(x + y + 1, 2);
  double t2 = 19 - 14 * x + 3 * x * x;
  double t3 = 14 * y + 6 * x * y + 3 * y * y;
  double t4 = 1 + t1 * (t2 - t3);

  double t5 = pow(2 * x - 3 * y, 2);
  double t6 = 18 - 32 * x + 12 * x * x;
  double t7 = 48 * y - 36 * x * y + 27 * y * y;
  double t8 = 30 + t5 * (t6 + t7);

  *f = t4 * t8;
}


void GoldsteinPrice_gradient(double x, double *xb, double y, double *yb, double *f, double *fb)
{
  double t1;
  double t1b;
  double tempb0;
  double tempb;
  t1 = pow(x + y + 1, 2);
  double t2 = 19 - 14*x + 3*x*x;
  double t2b;
  double t3 = 14*y + 6*x*y + 3*y*y;
  double t3b;
  double t4 = 1 + t1*(t2-t3);
  double t4b;
  double t5;
  double t5b;
  t5 = pow(2*x - 3*y, 2);
  double t6 = 18 - 32*x + 12*x*x;
  double t6b;
  double t7 = 48*y - 36*x*y + 27*y*y;
  double t7b;
  double t8 = 30 + t5*(t6+t7);
  double t8b;
  t4b = t8*(*fb);
  t8b = t4*(*fb);
  t5b = (t6+t7)*t8b;
  t6b = t5*t8b;
  t7b = t5*t8b;
  tempb = 2*pow(2*x-3*y, 2-1)*t5b;
  t1b = (t2-t3)*t4b;
  t2b = t1*t4b;
  t3b = -(t1*t4b);
  tempb0 = 2*pow(x+y+1, 2-1)*t1b;
  *yb = *yb + tempb0 - 3*tempb + (3*2*y+6*x+14)*t3b + (27*2*y-36*x+48)*t7b;
  *xb = *xb + (12*2*x-32)*t6b + 6*y*t3b + tempb0 + (3*2*x-14)*t2b + 2*tempb
    - 36*y*t7b;
  *fb = 0.0;
}


void GradientDescent(double &x, double &y)
{
  double tolerance = 1e-8;
  double alpha = 1e-6;

  double x0 = 0;
  double y0 = 0;

  for (auto it = 0; it != 100000; ++it)
  {
    x0 = x;
    y0 = y;

    double dfdx0 = 0;
    double dfdy0 = 0;
    double f = 0;

    // f
    GoldsteinPrice(x0, y0, &f);

    // df/dx, df/dy
    double dfdf = 1;
    GoldsteinPrice_gradient(x0, &dfdx0, y0, &dfdy0, &f, &dfdf);

    x = x0 - alpha * dfdx0;
    y = y0 - alpha * dfdy0;

    double norm = std::max(std::abs(x - x0), std::abs(y - y0));

    if (it % 1000 == 0)
    {
      std::cout << "iteration: " << it
                << ", x = " << x
                << ", y = " << y
                << ", f = " << f
                << ", norm = " << norm
                << std::endl;
    }

    if (norm < tolerance)
      break;
  }
}


int main()
{
  double x = -1.5;
  double y = 1.5;

  // minimum at: f(0, -1) = 3
  GradientDescent(x, y);
}
