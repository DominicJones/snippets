class Field
{
public:
  Field(double const &value)
  : _value(value)
  {}

  double operator[](int i) const {
    return _value * _value;
  }

private:
  double const &_value;
};


class Primal {};
class Tangent {};
class Adjoint {};

template<class Mode> class DrvField;

template<> class DrvField<Primal>
  : public Field
{
public:
  DrvField(double const &value)
    : Field(value)
  {}
};

template<> class DrvField<Tangent>
{
public:
  DrvField(double const &value, double const &value_drv)
    : _value(value)
    , _value_drv(value_drv)
  {}

  double operator[](int i) const {
    return 2 * _value * _value_drv;
  }

private:
  double const &_value;
  double const &_value_drv;
};

template<> class DrvField<Adjoint>
{
public:
  DrvField(double const &value, double &value_drv)
    : _value(value)
    , _value_drv(value_drv)
  {}

  class OpAddAssign
  {
  public:
    OpAddAssign(DrvField &outer)
      : _outer(outer)
    {}

    void operator+=(double const &rvalue_drv) const {
      _outer._value_drv += 2 * _outer._value * rvalue_drv;
    }

  private:
    DrvField &_outer;
  };

  OpAddAssign operator[](int i) {
    return OpAddAssign(*this);
  }

private:
  double const &_value;
  double &_value_drv;
};


#include <cassert>

int main()
{
  double const a(3);
  double const a_tng(1);
  double a_adj(0);

  double b(0);
  double b_tng(0);
  double const b_adj(1);

  {
    DrvField<Primal> field(a);
    b = field[0];
  }
  assert (a == 3);
  assert (b == 9);

  {
    DrvField<Tangent> field(a, a_tng);
    b_tng = field[0];
  }
  assert (b_tng == 6);

  {
    DrvField<Adjoint> field(a, a_adj);
    field[0] += b_adj;
  }
  assert (a_adj == 6);
}
