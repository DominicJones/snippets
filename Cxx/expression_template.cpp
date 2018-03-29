struct OpMultiply
{
  static double apply(double const &oprnd0, double const &oprnd1) {
    return oprnd0 * oprnd1;
  }
};


template<class Oprnd0, class Optr, class Oprnd1>
class OpBinary
{
  Oprnd0 const &oprnd0;
  Oprnd1 const &oprnd1;
public:
  OpBinary(Oprnd0 const &oprnd0, Oprnd1 const &oprnd1)
  : oprnd0(oprnd0), oprnd1(oprnd1) {}

  double operator[](int i) const {
    return Optr::apply(this->oprnd0[i], this->oprnd1[i]);
  }
};


template<class Oprnd0, class Oprnd1>
OpBinary<Oprnd0, OpMultiply, Oprnd1> operator*(Oprnd0 const &oprnd0, Oprnd1 const &oprnd1) {
  return OpBinary<Oprnd0, OpMultiply, Oprnd1>(oprnd0, oprnd1);
}


template<int length>
class Vector
{
  double data[length];
public:
  Vector(double data[length]) {
    for (int i = 0; i != length; ++i) this->data[i] = data[i];
  }

  template<class Oprnd0, class Optr, class Oprnd1>
  void operator=(OpBinary<Oprnd0, Optr, Oprnd1> const expr) {
    for (int i = 0; i != length; ++i) this->data[i] = expr[i];
  }

  double operator[](int i) const {
    return this->data[i];
  }
};


#include <cassert>
int main() {
  int const length(3);

  double a[] = { 1, 2, 3, };
  double b[] = { 4, 5, 6, };
  double c[] = { 7, 8, 9, };
  double d[length] = {0};

  Vector<length> w(a), x(b), y(c), z(d);
  z = w * x * y;

  assert(z[0] == (1*4*7));
  assert(z[1] == (2*5*8));
  assert(z[2] == (3*6*9));
}
