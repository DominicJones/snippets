#include <iostream>
#include <cmath>

typedef double Real;

class Adjoint {};


template<class T, class Mode> class DrvScalar;

template<class T> class DrvScalar<T, Adjoint>
{
  template<class TT> friend class OpAddAssign_pow;

private:
  class OpAddAssign_add {
    friend class DrvScalar;
  public:
    OpAddAssign_add(DrvScalar &op1, DrvScalar &op2)
      : _op1(op1), _op2(op2) {}
    void operator+=(DrvScalar const &res) const {
      _op1.drv() += res.drv();
      _op2.drv() += res.drv();
    }
  private:
    DrvScalar &_op1;
    DrvScalar &_op2;
  };

  class OpAddAssign_subt {
    friend class DrvScalar;
  public:
    OpAddAssign_subt(DrvScalar &op1, DrvScalar &op2)
      : _op1(op1), _op2(op2) {}
    void operator+=(DrvScalar const &res) const {
      _op1.drv() += res.drv();
      _op2.drv() += T(-1) * res.drv();
    }
  private:
    DrvScalar &_op1;
    DrvScalar &_op2;
  };

  class OpAddAssign_mult {
    friend class DrvScalar;
  public:
    OpAddAssign_mult(DrvScalar &op1, DrvScalar &op2)
      : _op1(op1), _op2(op2) {}
    void operator+=(DrvScalar const &res) const {
      _op1.drv() += _op2.pri() * res.drv();
      _op2.drv() += _op1.pri() * res.drv();
    }
  private:
    DrvScalar &_op1;
    DrvScalar &_op2;
  };

  class OpAddAssign_div {
    friend class DrvScalar;
  public:
    OpAddAssign_div(DrvScalar &op1, DrvScalar &op2)
      : _op1(op1), _op2(op2) {}
    void operator+=(DrvScalar const &res) const {
      _op1.drv() += res.drv() / _op2.pri();
      _op2.drv() += T(-1) * (_op1.pri() / _op2.pri()) * (res.drv() / _op2.pri());
    }
  private:
    DrvScalar &_op1;
    DrvScalar &_op2;
  };

public:
  DrvScalar(T const &pri)
    : _pri(pri), _drv(T(0)) {}

  OpAddAssign_add operator+(DrvScalar &op2) {
    return OpAddAssign_add(*this, op2);
  }

  OpAddAssign_subt operator-(DrvScalar &op2) {
    return OpAddAssign_subt(*this, op2);
  }

  OpAddAssign_mult operator*(DrvScalar &op2) {
    return OpAddAssign_mult(*this, op2);
  }

  OpAddAssign_div operator/(DrvScalar &op2) {
    return OpAddAssign_div(*this, op2);
  }

  void operator+=(T const &op1) {
    this->drv() += op1;
  }

  operator T() const {
    return this->drv();
  }

private:
  T const &pri() const { return _pri; }
  T const &drv() const { return _drv; }
  T &drv() { return _drv; }

  T const &_pri;
  T _drv;
};


template<class T> class OpAddAssign_pow {
public:
  OpAddAssign_pow(DrvScalar<T, Adjoint> &op1,
                  DrvScalar<T, Adjoint> &op2)
    : _op1(op1), _op2(op2) {}
  void operator+=(DrvScalar<T, Adjoint> const &res) const {
    _op1.drv() = pow(_op1.pri(), _op2.pri()) * (_op2.pri() / _op1.pri()) * res.drv();
    _op2.drv() = pow(_op1.pri(), _op2.pri()) * log(T(_op1.pri())) * res.drv();
  }
private:
  DrvScalar<T, Adjoint> &_op1;
  DrvScalar<T, Adjoint> &_op2;
};

template<class T>
OpAddAssign_pow<T> pow_drv(DrvScalar<T, Adjoint> &op1,
                           DrvScalar<T, Adjoint> &op2)
{
  return OpAddAssign_pow<T>(op1, op2);
}


size_t const size = 1;

Real const a_field[size] = {2};
Real const b_field[size] = {3};
Real z_field[size] = {0};

Real a_drv_field[size] = {0};
Real b_drv_field[size] = {0};
Real const z_drv_field[size] = {1};


int main()
{
  typedef DrvScalar<Real, Adjoint> DrvReal;
  for (size_t it = 0; it != size; ++it)
    {
      Real const a = a_field[it];
      Real const b = b_field[it];
      Real const p = a * b;
      Real const q = a / b;
      Real const r = a + p;
      Real const s = b - q;
      Real const z = pow(r, s);
      z_field[it] = z;

      DrvReal a_drv(a);
      DrvReal b_drv(b);
      DrvReal p_drv(p);
      DrvReal q_drv(q);
      DrvReal r_drv(r);
      DrvReal s_drv(s);
      DrvReal z_drv(z);

      z_drv += z_drv_field[it];
      pow_drv(r_drv, s_drv) += z_drv;
      b_drv - q_drv += s_drv;
      a_drv + p_drv += r_drv;
      a_drv / b_drv += q_drv;
      a_drv * b_drv += p_drv;
      a_drv_field[it] += a_drv;
      b_drv_field[it] += b_drv;
    }

  std::cout << z_field[0] << std::endl;
  std::cout << a_drv_field[0] << std::endl;
  std::cout << b_drv_field[0] << std::endl;
}
