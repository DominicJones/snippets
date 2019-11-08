// taken from: Kvasir MPL


#include <type_traits>


struct identity {
  template <typename T>
  using f = T;
};
namespace eager {
  template <typename T>
  using identity = T;
}


template <bool>
struct conditional;
template <>
struct conditional<true> {
  template <typename A, typename B>
  using f = A;
};
template <>
struct conditional<false> {
  template <typename A, typename B>
  using f = B;
};


template <typename M, typename C = identity>
struct same_as {
  template <typename T>
  using f = typename C::template f<std::is_same<M, T> >;
};
template <typename M>
struct same_as<M, identity> {
  template <typename T>
  using f = std::is_same<M, T>;
};


template <bool b, typename C>
struct dcall_impl;
template <typename C>
struct dcall_impl<true, C> : C {};
template <typename C, unsigned size>
using dcall = /*typename*/ dcall_impl<static_cast<bool>(size < 100000), C>;


template <typename C, typename... Ts>
using call = typename dcall<C, sizeof...(Ts)>::template f<Ts...>;



template<typename T> struct debug_t;
template<int v> struct debug_v;
int main()
{
  using t = same_as<double>::f<int>;
  debug_v<t::value>{};

  using u = conditional<t::value>::f<int, double>;
  debug_t<u>{};

  using v = call<same_as<double>, int>;
  debug_v<v::value>{};

  using w = call<conditional<v::value>, int, double>;
  debug_t<w>{};
}
