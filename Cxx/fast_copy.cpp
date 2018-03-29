template<typename T, int it>
struct Unroll
{
  inline static void copy(T const * const src, T * const dest)
  {
    Unroll<T, it - 1>::copy(src, dest);
    dest[it] = src[it];
  }
};

template<typename T>
struct Unroll<T, 0>
{
  inline static void copy(T const * const src, T * const dest)
  {
    dest[0] = src[0];
  }
};


struct S
{
  char c = 'a';
  int i = 2;
  double d = 3.14;
};


int main()
{
  S s;
  char s_cp[sizeof(S)];
  char * p = reinterpret_cast<char *>(&s);

  Unroll<char, sizeof(S)>::copy(p, s_cp);

  S * s_p = reinterpret_cast<S *>(s_cp);
  return s_p->i;
}
