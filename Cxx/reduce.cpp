#include <iostream>
#include <array>

/*
void reduce(...)
{
  Drv<mode, double> y0{y, y_drv};
  for (int i = 0; i != n; ++i)
  {
    Drv<mode, double&> y1{y0.mutate()};
    y1 += x[i];
  }
}
*/
    
auto reduce(int n, double * x) {
    double y{0};
    for (int i = 0; i != n; ++i) {
        y += x[i];
    }
    return y;
}

void reduce(int n, double * x, double * y) {
    *y = 0;
    for (int i = 0; i != n; ++i) {
        *y = *y + x[i];
    }
}

void reduce_d(int n, double *x, double *xd, double *yd) {
    *yd = 0;
    for (int i = 0; i < n; ++i) {
        *yd = *yd + xd[i];
    }
}

void reduce_b(int n, double *x, double *xb, double *yb) {
    for (int i = 0; i < n; ++i) {
        xb[i] = xb[i] + *yb;
    }
    *yb = 0;
}

int main()
{
  std::array<double, 3> x{1,2,3};
  double y;
  reduce(x.size(), x.data(), &y);
  std::cout << "reduce = " << y << std::endl;
  std::cout << "reduce = " << reduce(x.size(), x.data()) << std::endl;
}
