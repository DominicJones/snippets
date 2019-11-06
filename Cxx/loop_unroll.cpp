#include <type_traits>

template<int I> using int_ = std::integral_constant<int, I>;

template<int N>
void unroll(int_<N>, int_<N>)
{}

template<int N, int I = 0>
void unroll(int_<N> n, int_<I> = int_<0>{})
{
  unroll(n, int_<I + 1>{});
}

int main()
{
  unroll(int_<800>{});
}
