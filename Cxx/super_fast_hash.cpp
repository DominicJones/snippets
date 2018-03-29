// from: http://www.azillionmonkeys.com/qed/hash.html

#include <iostream>
#include <cassert>


inline unsigned int get16bits(char const * data)
{
  return (((unsigned int)(((const unsigned char *)(data))[1])) << 8) +
    (unsigned int)(((const unsigned char *)(data))[0]);
}


unsigned int superFastHash(char const * data, int const size, unsigned int const hash0)
{
  int len = size;
  unsigned int hash = hash0;
  unsigned int tmp;
  int rem;

  if (len <= 0 || data == 0)
    return 0;

  rem = len & 3;
  len >>= 2;

  for (;len > 0; len--)
  {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2 * (2 * sizeof(unsigned char));
    hash  += hash >> 11;
  }

  switch (rem)
  {
  case 3:
    hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= ((signed char)data[(2 * sizeof(unsigned char))]) << 18;
    hash += hash >> 11;
    break;
  case 2:
    hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1:
    hash += (signed char)*data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }

  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash;
}


static unsigned int const hash0 = 982451653;


unsigned int superFastHash(char const * data, int const &size)
{
  return superFastHash(data, size, hash0);
}


unsigned int superFastHash(unsigned int const &data)
{
  int const size = sizeof(unsigned int);
  return superFastHash(reinterpret_cast<char const *>(&data), size, hash0);
}


unsigned int superFastHash(unsigned int const &data0, unsigned int const &data1)
{
  int const len = 2;
  int const size = len * sizeof(unsigned int);
  unsigned int const data[len] = { data0, data1 };
  return superFastHash(reinterpret_cast<char const *>(&data), size, hash0);
}


unsigned int superFastHash(unsigned int const &data0, unsigned int const &data1, unsigned int const &data2)
{
  int const len = 3;
  int const size = len * sizeof(unsigned int);
  unsigned int const data[len] = { data0, data1, data2 };
  return superFastHash(reinterpret_cast<char const *>(&data), size, hash0);
}


int main()
{
  unsigned int const f(2);
  unsigned int const c0(5);
  unsigned int const c1(8);

  std::size_t const id0 = superFastHash(f, c0, c1);
  std::cout << id0 << std::endl;

  std::size_t const id1 = superFastHash(f, c1, c0);
  std::cout << id1 << std::endl;

  assert(id0 != id1);
}
