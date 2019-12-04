#include <tuple>

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


struct Density
{
  using Args = std::tuple<bool const, float const &>;

  Density(bool const &b, float const &f)
    : _b(b), _f(f) {}

  void update() {}

  bool const _b;
  float const &_f;
};


template<class Var>
struct Make
{
  using Args = typename Var::Args;

  template<typename... Args>
  Make(Args &&... args)
    : _args(args...)
  {}

  void update()
  {
    std17::make_from_tuple<Var>(_args).update();
  }

  Args const _args;
};


int main()
{
  bool b;
  float f;
  Make<Density>(b, f).update();
}
