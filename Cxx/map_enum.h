// -*- C++ -*-

#ifndef _Colour_h_
#define _Colour_h_

#include <map>
#include <string>

#define FOR_ALL_OPTIONS(apply)                  \
  apply(Red)                                    \
  apply(Blue)                                   \
  apply(Green)                                  \

struct Colour
{
#define SELECT_ENUM(a) a,
  enum Options { FOR_ALL_OPTIONS(SELECT_ENUM) };
#undef SELECT_ENUM

  static Options getOption(std::string key)
  {
    return (*_map.find(key)).second;
  }

  static std::map<std::string, Options> createMap()
  {
    std::map<std::string, Options> map_;
#define SELECT_MAP(a) map_[#a] = a;
    FOR_ALL_OPTIONS(SELECT_MAP);
#undef SELECT_MAP
    return map_;
  }

  static const std::map<std::string, Options> _map;
};


#endif
