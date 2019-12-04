#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>


namespace std17
{
  namespace detail
  {
    template<size_t N>
    struct ApplyFunction
    {
      template<typename F, typename T, typename... A>
      static inline auto apply(F && f, T && t, A &&... a)
      {
        return ApplyFunction<N-1>::apply(std::forward<F>(f), std::forward<T>(t), std::get<N-1>(std::forward<T>(t)), std::forward<A>(a)...);
      }
    };

    template<>
    struct ApplyFunction<0>
    {
      template<typename F, typename T, typename... A>
      static inline auto apply(F && f, T &&, A &&... a)
      {
        return std::forward<F>(f)(std::forward<A>(a)...);
      }
    };
  }

  template<typename F, typename T>
  inline auto apply(F && f, T && t) {
    return detail::ApplyFunction< std::tuple_size< std::decay_t<T> >::value>::apply(std::forward<F>(f), std::forward<T>(t));
  }


  namespace detail
  {
    template<size_t N>
    struct ApplyMember
    {
      template<typename C, typename F, typename T, typename... A>
      static inline auto apply(C&& c, F&& f, T&& t, A&&... a) ->
        decltype(ApplyMember<N-1>::apply(std::forward<C>(c), std::forward<F>(f), std::forward<T>(t), std::get<N-1>(std::forward<T>(t)), std::forward<A>(a)...))
      {
        return ApplyMember<N-1>::apply(std::forward<C>(c), std::forward<F>(f), std::forward<T>(t), std::get<N-1>(std::forward<T>(t)), std::forward<A>(a)...);
      }
    };

    template<>
    struct ApplyMember<0>
    {
      template<typename C, typename F, typename T, typename... A>
      static inline auto apply(C&& c, F&& f, T&&, A&&... a) ->
        decltype((std::forward<C>(c)->*std::forward<F>(f))(std::forward<A>(a)...))
      {
        return (std::forward<C>(c)->*std::forward<F>(f))(std::forward<A>(a)...);
      }
    };
  }

  template<typename C, typename F, typename T>
  inline auto apply(C&& c, F&& f, T&& t) ->
    decltype(detail::ApplyMember<std::tuple_size<typename std::decay<T>::type>::value>::apply(std::forward<C>(c), std::forward<F>(f), std::forward<T>(t)))
  {
    return detail::ApplyMember<std::tuple_size<typename std::decay<T>::type>::value>::apply(std::forward<C>(c), std::forward<F>(f), std::forward<T>(t));
  }
}
