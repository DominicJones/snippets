#include <array>
#include <iostream>


typedef double Real;
template<typename T, typename M> class Drv;

template<int N, typename T> class Vector;
template<int N, typename T, typename M> class DrvVector;


struct Tangent_t;
struct Adjoint_t;


template<int N, typename T> class Vector
{
public:
  Vector(std::array<T, N> const &data)
    : _data(data)
  {}

  Vector(T const &data)
    : _data((std::array<T, N>{data}))
  {}

  Real const &operator[](size_t const &index) const
  {
    return _data[index];
  }

  Real &operator[](size_t const &index)
  {
    return _data[index];
  }

  Vector const operator+(Vector const &op2) const
  {
    return Vector<N, T>((*this)[0] + op2[0]);
  }

  friend Vector const operator*(Real const &op1, Vector const &op2)
  {
    return Vector<N, T>(op1 + op2[0]);
  }

  Real const dot(Vector const &op2) const
  {
    return (*this)[0] * op2[0];
  }

  void operator+=(Vector const &op2)
  {
    _data[0] += op2[0];
  }

private:
  std::array<T, N> _data;
};


template<typename T> class Drv<T, Tangent_t>
{
public:
  Drv(T const &pri,
      T const &drv)
    : _pri(pri)
    , _drv(drv)
  {}

  T const &pri() const
  {
    return _pri;
  }

  T const &drv() const
  {
    return _drv;
  }

  operator T const &() const
  {
    return _drv;
  }

  T const operator+(Drv const &op2) const
  {
    return this->drv() + op2.drv();
  }

  T const operator*(Drv const &op2) const
  {
    return this->drv() * op2.pri() + this->pri() * op2.drv();
  }

private:
  T const &_pri;
  T const _drv;
};


template<typename T> class Drv<T, Adjoint_t>
{
public:
  Drv(T const &pri,
      T const &drv)
    : _pri(pri)
    , _drv(drv)
  {}

  Drv(T const &pri)
    : _pri(pri)
    , _drv(0)
  {}

  T const &pri() const
  {
    return _pri;
  }

  T const &drv() const
  {
    return _drv;
  }

  T &drv()
  {
    return _drv;
  }

  operator T const &() const
  {
    return _drv;
  }

  class OpAddAssign_opAdd {
  public:
    OpAddAssign_opAdd(Drv &op1,
                      Drv &op2)
      : _op1(op1)
      , _op2(op2)
    {}
    void operator+=(Drv const &result) const {
      _op1.drv() += result.drv();
      _op2.drv() += result.drv();
    }
  private:
    Drv &_op1;
    Drv &_op2;
  };

  OpAddAssign_opAdd operator+(Drv &op2)
  {
    return OpAddAssign_opAdd(*this, op2);
  }

  class OpAddAssign_opMultiply {
  public:
    OpAddAssign_opMultiply(Drv &op1,
                           Drv &op2)
      : _op1(op1)
      , _op2(op2)
    {}
    void operator+=(Drv const &result) const {
      _op1.drv() += _op2.pri() * result.drv();
      _op2.drv() += _op1.pri() * result.drv();
    }
  private:
    Drv &_op1;
    Drv &_op2;
  };

  OpAddAssign_opMultiply operator*(Drv &op2)
  {
    return OpAddAssign_opMultiply(*this, op2);
  }

private:
  T const &_pri;
  T _drv;
};


template<int N, typename T> class DrvVector<N, T, Tangent_t>
{
public:
  DrvVector(Vector<N, T> const &pri,
            Vector<N, T> const &drv)
    : _pri(pri)
    , _drv(drv)
  {}

  Vector<N, T> const &pri() const
  {
    return _pri;
  }

  Vector<N, T> const &drv() const
  {
    return _drv;
  }

  operator Vector<N, T> const &() const
  {
    return _drv;
  }

  T const &operator[](size_t const &index) const
  {
    return _drv[index];
  }

  Vector<N, T> const operator+(DrvVector const &op2) const
  {
    return Vector<N, T>(this->drv()[0] + op2.drv()[0]);
  }

  friend Vector<N, T> const operator*(Drv<T, Tangent_t> const &op1, DrvVector const &op2)
  {
    return Vector<N, T>(op1.drv() * op2.pri()[0] + op1.pri() * op2.drv()[0]);
  }

  T const dot(DrvVector const &op2) const
  {
    return T(this->drv()[0] * op2.pri()[0] + this->pri()[0] * op2.drv()[0]);
  }

private:
  Vector<N, T> const &_pri;
  Vector<N, T> const _drv;
};


template<int N, typename T> class DrvVector<N, T, Adjoint_t>
{
public:
  DrvVector(Vector<N, T> const &pri,
            Vector<N, T> const &drv)
    : _pri(pri)
    , _drv(drv)
  {}

  DrvVector(Vector<N, T> const &pri)
    : _pri(pri)
    , _drv(Vector<N, T>(0))
  {}

  Vector<N, T> const &pri() const
  {
    return _pri;
  }

  Vector<N, T> const &drv() const
  {
    return _drv;
  }

  Vector<N, T> &drv()
  {
    return _drv;
  }

  operator Vector<N, T> const &() const
  {
    return _drv;
  }

  class OpAddAssign_opBrackets {
  public:
    OpAddAssign_opBrackets(DrvVector &op1,
                           size_t const &index)
      : _op1(op1)
      , _index(index)
    {}
    void operator+=(Drv<T, Adjoint_t> const &result) const {
      _op1.drv()[_index] += result.drv();
    }
  private:
    DrvVector &_op1;
    size_t const &_index;
  };

  OpAddAssign_opBrackets operator[](size_t const &index)
  {
    return OpAddAssign_opBrackets(*this, index);
  }

  class OpAddAssign_opAdd {
  public:
    OpAddAssign_opAdd(DrvVector &op1,
                      DrvVector &op2)
      : _op1(op1)
      , _op2(op2)
    {}
    void operator+=(DrvVector const &result) const {
      _op1.drv()[0] += result.drv()[0];
      _op2.drv()[0] += result.drv()[0];
    }
  private:
    DrvVector &_op1;
    DrvVector &_op2;
  };

  OpAddAssign_opAdd operator+(DrvVector &op2)
  {
    return OpAddAssign_opAdd(*this, op2);
  }

  class OpAddAssign_opMultiply {
  public:
    OpAddAssign_opMultiply(Drv<T, Adjoint_t> &op1,
                           DrvVector &op2)
      : _op1(op1)
      , _op2(op2)
    {}
    void operator+=(DrvVector const &result) const {
      _op1.drv() += _op2.pri()[0] * result.drv()[0];
      _op2.drv()[0] += _op1.pri() * result.drv()[0];
    }
  private:
    Drv<T, Adjoint_t> &_op1;
    DrvVector &_op2;
  };

  friend OpAddAssign_opMultiply operator*(Drv<T, Adjoint_t> &op1, DrvVector &op2)
  {
    return OpAddAssign_opMultiply(op1, op2);
  }

  class OpAddAssign_dot {
  public:
    OpAddAssign_dot(DrvVector &op1,
                    DrvVector &op2)
      : _op1(op1)
      , _op2(op2)
    {}
    void operator+=(Drv<T, Adjoint_t> const &result) const {
      _op1.drv()[0] += _op2.pri()[0] * result.drv();
      _op2.drv()[0] += _op1.pri()[0] * result.drv();
    }
  private:
    DrvVector &_op1;
    DrvVector &_op2;
  };

  OpAddAssign_dot dot(DrvVector &op2)
  {
    return OpAddAssign_dot(*this, op2);
  }

private:
  Vector<N, T> const &_pri;
  Vector<N, T> _drv;
};


Vector<1, Real> const A(2);
Vector<1, Real> const B(3);
Real F(0);

Vector<1, Real> const A_tng(1);
Vector<1, Real> const B_tng(0);
Real F_tng(0);

Vector<1, Real> A_adj(0);
Vector<1, Real> B_adj(0);
Real const F_adj(1);


int test1()
{
  Vector<1, Real> const a(A);
  Vector<1, Real> const b(B);
  Vector<1, Real> const c(a + b);
  Real const d(b.dot(c));
  Real const e(a.dot(c));
  Real const f(d * e);
  F = f;
  std::cout << F << std::endl;

  {
    DrvVector<1, Real, Tangent_t> const a_drv(a, A_tng);
    DrvVector<1, Real, Tangent_t> const b_drv(b, B_tng);
    DrvVector<1, Real, Tangent_t> const c_drv(c, a_drv + b_drv);
    Drv<Real, Tangent_t> const d_drv(d, b_drv.dot(c_drv));
    Drv<Real, Tangent_t> const e_drv(e, a_drv.dot(c_drv));
    Drv<Real, Tangent_t> const f_drv(f, d_drv * e_drv);
    F_tng = f_drv;
    std::cout << F_tng << std::endl;
  }

  {
    DrvVector<1, Real, Adjoint_t> a_drv(a);
    DrvVector<1, Real, Adjoint_t> b_drv(b);
    DrvVector<1, Real, Adjoint_t> c_drv(c);
    Drv<Real, Adjoint_t> d_drv(d);
    Drv<Real, Adjoint_t> e_drv(e);
    Drv<Real, Adjoint_t> f_drv(f, F_adj);

    d_drv * e_drv += f_drv;
    a_drv.dot(c_drv) += e_drv;
    b_drv.dot(c_drv) += d_drv;
    a_drv + b_drv += c_drv;
    B_adj += b_drv;
    A_adj += a_drv;
    std::cout << B_adj[0] << std::endl;
    std::cout << A_adj[0] << std::endl;
  }
}


int test2()
{
  Vector<1, Real> const a(A);
  Vector<1, Real> const b(B);
  Real const c(a.dot(b));
  Vector<1, Real> const d(c * b);
  Real const f(d.dot(d));
  F = f;
  std::cout << F << std::endl;

  {
    DrvVector<1, Real, Tangent_t> const a_drv(a, A_tng);
    DrvVector<1, Real, Tangent_t> const b_drv(b, B_tng);
    Drv<Real, Tangent_t> const c_drv(c, a_drv.dot(b_drv));
    DrvVector<1, Real, Tangent_t> const d_drv(d, c_drv * b_drv);
    Drv<Real, Tangent_t> const f_drv(f, d_drv.dot(d_drv));
    F_tng = f_drv;
    std::cout << F_tng << std::endl;
  }

  {
    DrvVector<1, Real, Adjoint_t> a_drv(a);
    DrvVector<1, Real, Adjoint_t> b_drv(b);
    Drv<Real, Adjoint_t> c_drv(c);
    DrvVector<1, Real, Adjoint_t> d_drv(d);
    Drv<Real, Adjoint_t> f_drv(f, F_adj);

    d_drv.dot(d_drv) += f_drv;
    c_drv * b_drv += d_drv;
    a_drv.dot(b_drv) += c_drv;
    B_adj += b_drv;
    A_adj += a_drv;
    std::cout << B_adj[0] << std::endl;
    std::cout << A_adj[0] << std::endl;
  }
}


int main()
{
  // test1();
  test2();
}
