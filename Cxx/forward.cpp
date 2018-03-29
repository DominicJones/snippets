#include <utility>


class Variable
{
public:
  Variable(int &a, int const &b, int &&c, int d) {}
};


template<typename Var, typename... Args>
Var build(Args &&... args)
{
  return Var(std::forward<Args>(args)...);
}


int main()
{
  int a = 1;
  int const b = 2;
  auto variable(build<Variable>(a, b, 3, 4));
}
