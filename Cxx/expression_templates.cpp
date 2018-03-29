// from: http://en.wikipedia.org/wiki/Expression_templates

#include <iostream>
#include <vector>
#include <cassert>


template <typename E>
class VecExpression
{
public:
  typedef std::vector<double>         container_type;
  typedef container_type::size_type   size_type;
  typedef container_type::value_type  value_type;
  typedef container_type::reference   reference;

  size_type  size()                  const { return static_cast<E const&>(*this).size(); }
  value_type operator[](size_type i) const { return static_cast<E const&>(*this)[i];     }

  operator E&()             { return static_cast<      E&>(*this); }
  operator E const&() const { return static_cast<const E&>(*this); }
};


class Vec : public VecExpression<Vec>
{
public:
  reference  operator[](size_type i)       { return _data[i]; }
  value_type operator[](size_type i) const { return _data[i]; }
  size_type  size()                  const { return _data.size(); }

  Vec(container_type const &vec) : _data(vec) {}

  template <typename E>
  Vec(VecExpression<E> const& vec)
  {
    E const& v = vec;
    _data.resize(v.size());
    for (size_type i = 0; i != v.size(); ++i)
      {
        _data[i] = v[i];
      }
  }

private:
  container_type _data;
};


template <typename E1, typename E2>
class VecDifference : public VecExpression<VecDifference<E1, E2> >
{
public:
  typedef Vec::size_type size_type;
  typedef Vec::value_type value_type;

  VecDifference(VecExpression<E1> const& u, VecExpression<E2> const& v)
    : _u(u), _v(v)
  {
    assert(u.size() == v.size());
  }

  size_type size() const { return _v.size(); }
  value_type operator[](Vec::size_type i) const { return _u[i] - _v[i]; }

private:
  E1 const& _u;
  E2 const& _v;
};


template <typename E>
class VecScaled : public VecExpression<VecScaled<E> >
{
public:
  VecScaled(double alpha, VecExpression<E> const& v)
    : _alpha(alpha), _v(v)
  {}

  Vec::size_type size() const { return _v.size(); }
  Vec::value_type operator[](Vec::size_type i) const { return _alpha * _v[i]; }

private:
  double _alpha;
  E const& _v;
};


template <typename E1, typename E2>
VecDifference<E1, E2> const
operator-(VecExpression<E1> const& u, VecExpression<E2> const& v)
{
  return VecDifference<E1, E2>(u, v);
}


template <typename E>
VecScaled<E> const
operator*(double alpha, VecExpression<E> const& v)
{
  return VecScaled<E>(alpha, v);
}


int main()
{
  double alpha(5);
  Vec u(Vec::container_type(2, 20));
  Vec v(Vec::container_type(2, 10));
  Vec x = alpha * (u - v);
  std::cout << x[0] << " " << x[1] << std::endl;
}
