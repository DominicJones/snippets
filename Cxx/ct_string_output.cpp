struct ct_str
{
  char _data[512]{};
  std::size_t _size{0};
  
  template<std::size_t N> constexpr ct_str(char const (&str)[N])
  : _data{}, _size{N-1}
  {
    for (std::size_t it = 0; it != _size; ++it)
      _data[it] = str[it];
  }
};

template<ct_str> struct print;

print<"test"> p1{};
ct_str test()
{
  //        0         1
  //        012345678901
  ct_str s{"test@line:LL"};
  s._data[10] = 2;
  s._data[11] = 0;
  return s;
}

print<test()> p2{};
