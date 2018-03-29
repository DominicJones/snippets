#include <type_traits>


template<typename T>
class has_eval
{
private:
  template<typename C, typename P>
  static auto test(P * p) -> decltype(std::declval<C>().eval(*p), std::true_type());

  template<typename, typename>
  static std::false_type test(...);

  using type = decltype(test<T, std::nullptr_t>(nullptr));

public:
  static const bool value = std::is_same<std::true_type, type>::value;
};


struct HasEval { template<typename T> void eval(T t) {} };
struct HasNotEval {};


static_assert(has_eval<HasEval>::value, "");
static_assert(!has_eval<HasNotEval>::value, "");


int main() {}
