// Taken from: https://github.com/kvasir-io/mpl




// #include <kvasir/mpl/functional/identity.hpp>
namespace kvasir {
  namespace mpl {
    /// \brief results in the dynamic input. Use to break recursive chains or forward args /
    /// identity is one of the core concepts of cmpl. It is used as the default continuation and
    /// when reached it breaks the chain as it has not continuation it self. it also comes in
    /// handy as a place holder in each if one only wants to modify some of the arguments. It is
    /// also used as a default predicate in algorithms like all where a predicate may be
    /// unneeded.
    struct identity {
      template <typename T>
      using f = T;
    };
    namespace eager {
      template <typename T>
      using identity = T;
    }
  } // namespace mpl
} // namespace kvasir




// #include <kvasir/mpl/compatability/dependent_call.hpp>
namespace kvasir {
  namespace mpl {
    namespace detail {
      template <bool b, typename C>
      struct dependant_impl;
      template <typename C>
      struct dependant_impl<true, C> : C {};
    } // namespace detail

    template <typename C, unsigned size>
    using dcall = typename detail::dependant_impl<static_cast<bool>(size < 100000), C>;

    template <bool>
    struct dcallf;
    template <>
    struct dcallf<true> {
      template <template <typename...> class F1, typename... Ts>
      using f1 = F1<Ts...>;
      template <template <typename...> class F1, template <typename...> class F2,
                typename... Ts>
      using f2 = F1<F2<Ts...>>;
    };
    template <>
    struct dcallf<false> {
      template <template <typename...> class F1, typename... Ts>
      using f1 = F1<>;
      template <template <typename...> class F1, template <typename...> class F2,
                typename... Ts>
      using f2 = F1<F2<>>;
    };
  } // namespace mpl
} // namespace kvasir




// #include <kvasir/mpl/functional/call.hpp>
namespace kvasir {
  namespace mpl {
    namespace detail {
      template <typename C, typename L, typename... Ts>
      struct unpack_impl;
      template <typename C, template <typename...> class Seq, typename... Ls, typename... Ts>
      struct unpack_impl<C, Seq<Ls...>, Ts...> {
        using type = typename dcall<C, sizeof...(Ls)>::template f<Ts..., Ls...>;
      };
    } // namespace detail
    /// \brief turns a list of types into a variadic pack of those types /
    /// example: call<all<>,true_,false_,true_> is equivalent to
    /// call<unpack<all<>>,list<true_,false_,true_>>
    template <typename C>
    struct unpack {
      template <typename... Ls>
      using f = typename detail::unpack_impl<C, Ls...>::type;
    };

    /// \brief call a continuation (left parameter) with a variadic pack
    template <typename C, typename... Ts>
    using call = typename dcall<C, sizeof...(Ts)>::template f<Ts...>;

    template <typename C = identity>
    struct call_f {
      template <typename... Ts>
      using f = typename C::template f<
        dcallf<(sizeof...(Ts) < 10000)>::template f1<call, Ts...>>;
    };
    /// \exclude
    template <>
    struct call_f<identity> {
      template <typename... Ts>
      using f = typename dcallf<(sizeof...(Ts) < 10000)>::template f1<call, Ts...>;
    };
  } // namespace mpl
} // namespace kvasir




// #include <kvasir/mpl/functional/bind.hpp>
namespace kvasir {
  namespace mpl {
    /// \brief makes a continuation from a lazy metafunction /
    /// wraps a template template parameter in such a way that it can be used as a
    /// continuation capable meta closure.
    /// \requires fixed parameters: a template template parameter which contains a nestesd
    /// ::type (the lazy metafunction) dynamic parameters: a variadic pack of type parameters
    /// \effects the result will be whatever the ::type of the lazy metafunction maps to when
    /// passed the dynamic parameters
    template <template <typename...> class F, typename C = identity>
    struct cfl {
      template <typename... Ts>
      using f = typename dcall<C, sizeof...(Ts)>::template f<
        typename dcallf<bool(sizeof...(Ts) > 0)>::template f1<F, Ts...>::type>;
    };
    /// \exclude
    template <template <typename...> class F>
    struct cfl<F, identity> {
      template <typename... Ts>
      using f = typename dcallf<bool(sizeof...(Ts) > 0)>::template f1<F, Ts...>::type;
    };

    /// \brief makes a continuation from an eager metafunction /
    /// wraps a template template parameter in such a way that it can be used as a
    /// continuation capable meta closure.
    /// \requires fixed parameters: a template template parameter (the eager metafunction)
    /// dynamic parameters: a variadic pack of type parameters
    /// \effects the result will be whatever the eager metafunction maps to when passed the
    /// dynamic parameters
    /// \notes when passed an alias the result is the same as a traditional call to the at
    /// metafunction/ \notes it is perfectly valid to pass a template such as tuple or pair as a
    /// 'metafunction'
    template <template <typename...> class F, typename C = identity>
    struct cfe {
      template <typename... Ts>
      using f = typename dcall<C, sizeof...(Ts)>::template f<
        typename dcallf<bool(sizeof...(Ts) > 0)>::template f1<F, Ts...>>;
    };
    /// \exclude
    template <template <typename...> class F>
    struct cfe<F, identity> {
      template <typename... Ts>
      using f = typename dcallf<bool(sizeof...(Ts) > 0)>::template f1<F, Ts...>;
    };
  } // namespace mpl
} // namespace kvasir




// #include <kvasir/mpl/types/list.hpp>
namespace kvasir {
  namespace mpl {
    template <typename... Ts>
    struct list {};
    namespace detail {
      // recursive list, for internal use only
      template <typename Head, typename Tail>
      struct rlist {};

      using rlist_tail_of8 = rlist<
        list<>,
        rlist<list<>,
              rlist<list<>,
                    rlist<list<>,
                          rlist<list<>,
                                rlist<list<>, rlist<list<>, rlist<list<>, void>>>>>>>>;
    } // namespace detail
    using listify = cfe<list>;

    template <typename S>
    struct sequencify;
    template <template <typename...> class S, typename... Ts>
    struct sequencify<S<Ts...>> {
      using type = cfe<S>;
    };

    template <typename... Ts>
    list<Ts...> make_list(Ts...) {
      return {};
    }
  } // namespace mpl
} // namespace kvasir




// #include <kvasir/mpl/sequence/push_front.hpp>
namespace kvasir {
  namespace mpl {
    template <typename Input, typename C = listify>
    struct push_front {
      template <typename... Ts>
      using f = typename dcall<C, sizeof...(Ts)>::template f<Input, Ts...>;
    };
    namespace eager {
      namespace impl {
        template <typename Elem, typename List>
        struct push_front_impl;

        /// kvasir::mpl::list implementation
        template <typename Elem, typename... Ts>
        struct push_front_impl<Elem, mpl::list<Ts...>> {
          using f = mpl::list<Elem, Ts...>;
        };
      } // namespace impl

      /// push an element to the back of a list
      template <typename Elem, typename List>
      using push_front = typename impl::push_front_impl<Elem, List>::f;
    } // namespace eager
  } // namespace mpl
} // namespace kvasir




// #include <kvasir/mpl/algorithm/fold_left.hpp> (simplified)
namespace kvasir {
  namespace mpl {
    namespace detail {
      static constexpr int select_fold(const int in) {
        return in >= 4 ? 4 : in >= 2 ? 2 : in == 1 ? 1 : 0;
      }
      template <int>
      struct fold_impl;
      template <>
      struct fold_impl<0> {
        template <template <typename...> class F, typename In>
        using f = In;
      };
      template <>
      struct fold_impl<1> {
        template <template <typename...> class F, typename In, typename T0>
        using f = F<In, T0>;
      };
      template <>
      struct fold_impl<2> {
        template <template <typename...> class F, typename In, typename T0, typename T1,
                  typename... Ts>
        using f = typename fold_impl<select_fold(sizeof...(Ts))>::template f<F, F<F<In, T0>, T1>, Ts...>;
      };
      template <>
      struct fold_impl<4> {
        template <template <typename...> class F, typename In, typename T0, typename T1,
                  typename T2, typename T3, typename... Ts>
        using f = typename fold_impl<select_fold(sizeof...(Ts))>::template f<F, F<F<F<F<In, T0>, T1>, T2>, T3>, Ts...>;
      };

    } // namespace detail
    /// \brief folds left over a list using a binary predicate /
    /// fold left consideres the first element in the input pack the state, use `push_front<>`
    /// to add state if needed
    template <typename F, typename C = identity>
    struct fold_left {
      template <typename... Ts>
      using f = typename C::template f<typename detail::fold_impl<detail::select_fold(sizeof...(Ts)-1)>::template f<F::template f, Ts...>>;
    };
    /// \exclude
    template <template <typename...> class F, typename C>
    struct fold_left<cfe<F, identity>, C> {
      template <typename... Ts>
      using f = typename C::template f<typename detail::fold_impl<detail::select_fold(sizeof...(Ts)-1)>::template f<F, Ts...>>;
    };

    namespace eager {
      template <typename List, typename State, template <typename...> class Func>
      using fold_left = call<unpack<mpl::push_front<State, mpl::fold_left<cfe<Func>>>>, List>;
    }
  } // namespace mpl
} // namespace kvasir




// #include <kvasir/mpl/types/int.hpp>
namespace kvasir {
  namespace mpl {
    template <unsigned long long Value>
    struct uint_ {
      static constexpr unsigned long long value = Value;
    };

    template <long long Value>
    struct int_ {
      static constexpr long long value = Value;
    };
  } // namespace mpl
} // namespace kvasir




// #include "algorithm/fold_left.hpp"
#include <type_traits>

namespace
{
  using namespace kvasir;
  using mpl::uint_;

  template <typename T1, typename T2>
  using add = uint_<(T1::value + T2::value)>;

  using list_ = mpl::list<uint_<1>, uint_<2>, uint_<3>, uint_<4> >;
  static_assert(std::is_same<mpl::eager::fold_left<list_, uint_<0>, add>, uint_<10> >::value);
}

int main() {}
