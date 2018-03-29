// An example of using std::type_traits
// and hash indentification from std::typeinfo
//
// Note:
// In C++11 hash_code() cannot be evaluated at
// compile-time. For C++14, this restriction is
// lifted

#include <cassert>
#include <cstring>
#include <typeinfo>
#include <type_traits>


class T1 {};
class T2 : public T1 {};

typedef std::common_type<T1, T2>::type T;


int main()
{
  size_t T_id = typeid(T).hash_code();
  assert(T_id == typeid(T1).hash_code());
}
