// Taken from:
// http://web.archive.org/web/20110816070301/http://codepad.org/ZPif76MW

#include <vector>
#include <cassert>
#include <iostream>
#include <boost/timer.hpp>
#include <boost/proto/proto.hpp>

namespace std
{
  // Ugly hack, just for debugging purposes.
  // Dont do this in production code.
  template<typename T, typename A>
  ostream & operator<< (ostream & sout, vector<T, A> const & v)
  {
    sout << "vector{";
    for(std::size_t i = 0; i < v.size(); ++i )
      sout << (i? ", ": "") << v[i];
    return sout << "}";
  }
}

namespace linear_algebra
{
  using namespace boost;
  using namespace proto;

  // A trait that returns true only for std::vector
  template<typename T>
  struct is_std_vector:
    mpl::false_
  {};

  template<typename T, typename A>
  struct is_std_vector<std::vector<T, A> >:
    mpl::true_
  {};

  // A type used as a domain for linear algebra expressions
  struct linear_algebra_domain:
    domain<>
  {};

  // Define all the operator overloads for combining std::vectors
  BOOST_PROTO_DEFINE_OPERATORS(is_std_vector, linear_algebra_domain)

  // Take any expression and turn each node
  // into a subscript expression, using the
  // state as the RHS.
  struct Distribute:
    or_<
      when<terminal<_>, _make_subscript(_, _state)>,
      plus<Distribute, Distribute>
    >
  {};

  struct Optimize:
    or_<
      when<
        subscript<Distribute, terminal<_> >,
        Distribute(_left, _right)
      >,
      plus<Optimize, Optimize>,
      terminal<_>
    >
  {};
}

namespace eager_ops
{
  // Dumb, eager vector addition.
  template<typename T, typename A>
  std::vector<T,A> operator+(std::vector<T,A> left, std::vector<T,A> const &right)
  {
    assert(left.size() == right.size());
    for (size_t i = 0, n = left.size(); i < n; ++i)
      left[i] += right[i];
    return left;
  }
}

static const int celems = 4;
static const int cloops = 50000000;
static int const value[celems] = {1,2,3,4};
std::vector<int> A(value, value+celems), B(A);

int time_eager()
{
    using namespace eager_ops;
    int ret = 0;
    boost::timer tim;
    tim.restart();
    for (int i = 0; i < cloops; ++i)
      ret += (A + B)[3];
    double d = tim.elapsed();
    std::cout << "Eager evaluation: " << d << std::endl;
    return ret;
}

int time_lazy()
{
    using namespace linear_algebra;

    proto::default_context ctx;

    int ret = 0;
    boost::timer tim;
    tim.restart();
    for (int i = 0; i < cloops; ++i)
      ret += proto::eval(Optimize()((A + B)[3]), ctx);
    double d = tim.elapsed();
    std::cout << "Lazy evaluation: " << d << std::endl;
    return ret;
}

int main()
{
  std::cout << "Eager result: " << time_eager() << std::endl;
  std::cout << "Lazy result: " << time_lazy() << std::endl;
}
