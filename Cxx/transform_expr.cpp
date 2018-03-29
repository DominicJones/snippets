// taken from:
// http://stackoverflow.com/questions/18213249/transforming-an-expression-template-tree


#include <iostream>


template<typename L, typename R>
struct Binary
{
  L left;
  R right;
};


template<typename L, typename R>
std::ostream &operator<<(std::ostream &os, Binary<L, R> const &expr)
{
  os << "(" << expr.left << "*" << expr.right << ")";
  return os;
}


// NOTE: removed reference as universal-ref yields a reference type for lvalues!
template<typename L, typename R>
Binary<typename std::remove_reference<L>::type,
       typename std::remove_reference<R>::type>
operator*(L&& l, R&& r)
{
  return {std::forward<L>(l), std::forward<R>(r)};
}


template<typename Expr>
auto transform(const Expr &expr) -> Expr
{
  return expr;
}


// transform
//   Binary<Binary<LL, LR>, R>
// to
//   Binary<LL, Binary<LR, R> >
template<typename LL, typename LR, typename R>
auto transform(Binary<Binary<LL, LR>, R> const &expr)
  -> decltype(transform(Binary<LL, Binary<LR, R> >{expr.left.left, {expr.left.right, expr.right}}))
{
  return transform(Binary<LL, Binary<LR, R> >{expr.left.left, {expr.left.right, expr.right}});
}


int main()
{
  Binary<int, int> _1_2{1,2};
  auto res = _1_2*3*4*5*6;

  std::cout << res << std::endl;
  // prints: (((((1*2)*3)*4)*5)*6)

  std::cout << transform(res) << std::endl;
  // prints: (1*(2*(3*(4*(5*6)))))
}
