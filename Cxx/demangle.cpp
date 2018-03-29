// -*- C++ -*-

#ifndef _demangle_h_
#define _demangle_h_

#include <typeinfo>
#include <iostream>
#include <cstdlib>
#include <memory>

#if defined(__GNUG__)
#include <cxxabi.h>
struct Demangle
{
  static std::string eval(char const * name)
  {
    int status(-4);
    char * realname;

    realname = abi::__cxa_demangle(name, 0, 0, &status);

    std::string result(realname);
    free(realname);
    return result;
  }
};

template<typename Expr_t>
std::string demangle(Expr_t const &expr)
{
  return Demangle::eval(typeid(expr).name());
}

template<typename Expr_t>
std::string demangle()
{
  return Demangle::eval(typeid(Expr_t).name());
}
#endif

#endif // _demangle_h_


int main()
{
  int const a = 2;
  int const &b = a;

  std::cout << demangle(a) << std::endl;
  std::cout << demangle(b) << std::endl;
}
