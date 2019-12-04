#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace std17
{
  namespace detail
  {
    template <class T, class Tuple, size_t... Is>
    T make_from_tuple(Tuple &&tuple, std::index_sequence<Is...>)
    {
      return T{std::get<Is>(std::forward<Tuple>(tuple))...};
    }
  }

  template <class T, class Tuple>
  T make_from_tuple(Tuple &&tuple)
  {
    return detail::make_from_tuple<T>(std::forward<Tuple>(tuple),
                                      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple> >::value>{});
  }
}
