#include <type_traits>
template<typename> struct debug;


struct multiply
{
  template<auto A, auto B> static auto constexpr apply() { return A * B; };
};


template<auto... I> struct vlist;


template<typename L, auto I>
struct push_front;

template<template<auto...> typename L, auto... Is, auto I0>
struct push_front<L<Is...>, I0>
{
  using type = L<I0, Is...>;
};


template<typename OP, typename... S>
struct zip_pair;

template<typename OP,
         template<auto...> typename S,
         template<auto...> typename S0>
struct zip_pair<OP, S<>, S0<> >
{
  using type = S<>;
};

template<typename OP,
         template<auto...> typename S, auto I, auto... Is,
         template<auto...> typename S0, auto I0, auto... I0s>
struct zip_pair<OP, S<I, Is...>, S0<I0, I0s...> >
{
  static auto constexpr value_ = OP::template apply<I, I0>();
  using inner_ = typename zip_pair<OP, S<Is...>, S0<I0s...> >::type;
  using type = typename push_front<inner_, value_>::type;
};


template<typename OP, typename... S>
struct zip_combine;

template<typename OP, typename S>
struct zip_combine<OP, S>
{
  using type = S;
};

template<typename OP, typename S, typename S0, typename... Ss>
struct zip_combine<OP, S, S0, Ss...>
{
  using inner_ = typename zip_pair<OP, S, S0>::type;
  using type = typename zip_combine<OP, inner_, Ss...>::type;
};


template<typename... S>
struct zip
{
  using op = multiply;
  using type = typename zip_combine<op, S...>::type;
};


template<auto... Is> using Vector = vlist<Is...>;


// problem:
//   implement zip<Vector<values...>, ...>
//   such that its ::type member returns the multiplied reduction
int main()
{
  using Test = zip<Vector<1, 2, 3>,
                   Vector<2, 3, 4>,
                   Vector<3, 4, 5> >::type;
  using Expected = Vector<6, 24, 60>;
  static_assert(std::is_same_v<Test, Expected>);
}
