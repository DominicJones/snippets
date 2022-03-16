#include <functional>
#include <type_traits>


template<typename OP, typename E0> struct Unary
{
  Unary(E0 &&e0) : e0(std::forward<E0>(e0)) {}
  E0 e0;
};

struct Plus;

template<typename E0> auto plus(E0 &&e0)
{
  return Unary<Plus, E0>(std::forward<E0>(e0));
}



template<typename T = int> struct Value
{
  auto rplus() const { return ::plus(*this); }
  auto vplus() const { return ::plus(Value(*this)); }
};


int main()
{
  Value<> const value;

  auto mr = Value<>().rplus();
  auto mv = Value<>().vplus();

  auto er = plus(value);
  auto ev = plus(Value<>{});

  static_assert(std::is_same<decltype(mr), Unary<Plus, const Value<>&>>::value, "");
  static_assert(std::is_same<decltype(mv), Unary<Plus, Value<>>>::value, "");

  static_assert(std::is_same<decltype(er), Unary<Plus, const Value<>&>>::value, "");
  static_assert(std::is_same<decltype(ev), Unary<Plus, Value<>>>::value, "");
}
