// -*- C++ -*-

#ifndef _HashStdArray_h_
#define _HashStdArray_h_

#include <functional>

/*
  Specializations of std::hash for user-defined types must satisfy Hash requirements.
  In particular, they define an operator() that:
    1. Accepts a single parameter of type Key.
    2. Returns a value of type size_t that represents the hash value of the parameter.
    3. Does not throw exceptions when called.

  http://en.cppreference.com/w/cpp/language/extending_std
 */
namespace std
{
  template<typename T, size_t N>
  struct hash<array<T, N> >
  {
    using argument_type = array<T, N>;
    using result_type = size_t;

    inline result_type operator()(argument_type const &arg) const noexcept
    {
      result_type result = 0;
      hash<T> const hash_fn;

      // boost::hash_combine
      for (size_t it = 0; it != N; ++it)
      {
        result ^= hash_fn(arg[it]) + 0x9e3779b9 + (result << 6) + (result >> 2);
      }

      return result;
    }
  };
}

#endif // _HashStdArray_h_
