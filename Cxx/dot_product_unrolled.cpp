// g++ -std=c++11 -O3 <file>

#include <vector>
#include <numeric>
#include <iostream>

template <typename T, int size, int pos> struct dot_product_
{
  static T execute(T const (&a)[size], T const (&b)[size])
  {
    return a[pos] * b[pos] + dot_product_<T, size, pos - 1>::execute(a, b);
  }
};

template <typename T, int size> struct dot_product_<T, size, 0>
{
  static T execute(T const (&a)[size], T const (&b)[size])
  {
    return a[0] * b[0];
  }
};

template <typename T, int size> struct dot_product
{
  static T execute(T const (&a)[size], T const (&b)[size])
  {
    return dot_product_<T, size, size - 1>::execute(a, b);
  }
};

int main()
{
  auto const size = 800;

  auto a = new std::vector<double>(size);
  auto b = new std::vector<double>(size);
  a->assign(size, 0.256);
  b->assign(size, 0.512);

  double aa[size];
  double bb[size];
  for (size_t i = 0; i != size; ++i)
    {
      aa[i] = a->data()[i];
      bb[i] = b->data()[i];
    }

  auto c = std::inner_product(a->begin(), a->end(), b->begin(), double(0));
  std::cout << "dot-product (v1) = " << c << std::endl;

  auto cc = dot_product<double, size>::execute(aa, bb);
  std::cout << "dot-product (v2) = " << cc << std::endl;
}
