#include <iostream>

int const tangent =  1;
int const adjoint = -1;

double x = 2, x_drv;
double y = 3, y_drv;
double z = 5, z_drv;
double f, f_drv;

void eval(int mode)
{
  int const nb_cases = 2;
  int case_id = (nb_cases * (1 - mode)) / 2;

  double w1_drv(0);
  double const w1 = x * y;

  double w2_drv(0);
  double const w2 = y * z;

  f = w1 + w2;

 start: switch (case_id) {
  case 0:
    if (mode == tangent) {
      w1_drv = x_drv * y + x * y_drv;
    } else if (mode == adjoint) {
      x_drv += y * w1_drv;
      y_drv += x * w1_drv;
    }
    case_id += mode; goto start;
  case 1:
    if (mode == tangent) {
      w2_drv = y_drv * z + y * z_drv;
    } else if (mode == adjoint) {
      y_drv += z * w2_drv;
      z_drv += y * w2_drv;
    }
    case_id += mode; goto start;
  case 2:
    if (mode == tangent) {
      f_drv = w1_drv + w2_drv;
    } else if (mode == adjoint) {
      w1_drv += f_drv;
      w2_drv += f_drv;
    }
    case_id += mode; goto start;
  default:
    break;
  }
}

int main()
{
  x_drv = 1; y_drv = 0; z_drv = 0;
  eval(tangent);
  std::cout << f_drv << std::endl;

  f_drv = 1;
  x_drv = y_drv = z_drv = 0;
  eval(adjoint);
  std::cout << x_drv << std::endl;
}
