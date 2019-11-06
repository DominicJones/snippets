// https://odinthenerd.blogspot.com/2016/09/meta-monads.html?showComment=1573037622682


constexpr int selector(const int i)
{
  return i < 1 ? 0 : i < 8 ? 1 : 8;
}


template<int I>
struct at_generator;

template<>
struct at_generator<8>
{
  template<int I,
           typename T0,
           typename T1,
           typename T2,
           typename T3,
           typename T4,
           typename T5,
           typename T6,
           typename T7,
           typename...Ts>
  using type = typename at_generator<selector(I - 8)>::template type<I - 8, Ts...>;
};

template<>
struct at_generator<1>
{
  template<int I, typename T, typename...Ts>
  using type = typename at_generator<selector(I - 1)>::template type<I - 1, Ts...>;
};

template<>
struct at_generator<0>
{
  template<int I, typename T, typename...Ts>
  using type = T;
};


template<int I, typename L>
struct at_impl;

template<int I, template<typename...> class L, typename...Ts>
struct at_impl<I, L<Ts...> >
{
  using type = typename at_generator<selector(I)>::template type<I, Ts...>;
};

template<int I, typename L> using at = typename at_impl<I, L>::type;


template<typename...Ts> struct list;
template<typename T> struct debug_t;
template<int v> struct debug_v;


int main()
{
  using l = list<bool, char, int, float, double,
                 list<bool>, list<char>, list<int>, list<float>, list<double> >;

  debug_t<at<0, l> >{};
  debug_t<at<1, l> >{};
  debug_t<at<2, l> >{};
  debug_t<at<3, l> >{};
  debug_t<at<4, l> >{};
  debug_t<at<5, l> >{};
  debug_t<at<6, l> >{};
  debug_t<at<7, l> >{};
  debug_t<at<8, l> >{};
  debug_t<at<9, l> >{};

  debug_v<selector(0)>{};
  debug_v<selector(1)>{};
  debug_v<selector(2)>{};
  debug_v<selector(3)>{};
}
