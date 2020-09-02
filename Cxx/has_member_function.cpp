#include <type_traits>


template<typename T>
class has_at
{
private:
  template<typename C> static std::true_type test(decltype(&C::at));
  template<typename C> static std::false_type test(...);

public:
  static auto constexpr value = decltype(test<T>(nullptr))::value;
};


struct HasMemberFunction { void at() {} };
struct HasNotMemberFunction {};


int main()
{
  static_assert(has_at<HasMemberFunction>::value, "");
  static_assert(!has_at<HasNotMemberFunction>::value, "");
}
