#include <iostream>
#include <functional>


template<class T>
inline void hash_combine(std::size_t &seed, T const &value)
{
    std::hash<T> hasher;
    seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


int main()
{
  int a = 1;
  int b = 2;
  size_t hash = 0;
  hash_combine(hash, a);
  hash_combine(hash, b);
  std::cout << hash << std::endl;
}
