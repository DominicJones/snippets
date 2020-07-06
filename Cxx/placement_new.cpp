#include <new>
#include <iostream>
#include <vector>


enum UninitializedToken { UNINITIALIZED, };


template<int N, typename T>
struct Vector
{
  inline Vector() : v() {}
  inline Vector(UninitializedToken const &) {}
  template<typename I> inline T &operator()(I const &i) { return v[i]; }
  T v[N];
};


template<int N1, int N0, class T>
inline auto &
cast(Vector<N0, T> &v0)
{
  static_assert(N1 <= N0, "");
  return *::new(&v0) Vector<N1, T>(UNINITIALIZED);
}


int main()
{
  auto vv = std::vector<Vector<3, double>>();

  for (auto i = 0; i != 2; ++i)
  {
    auto v0 = Vector<3, double>();
    v0(0) = 2*(i+1); v0(1) = 3*(i+1); v0(2) = 4*(i+1);
    vv.push_back(v0);
  }

  auto &v1 = cast<2>(vv[0]);
  v1(1) = 5;

  for (auto i = 0; i != 2; ++i)
    std::cout << vv[i](0) << " " << vv[i](1) << " " << vv[i](2) << std::endl;
}
