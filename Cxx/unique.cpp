template<std::size_t ID, typename T> struct Unique
{
  operator T() const { return value; }
  T value;
};


template<std::size_t ID, typename T>
std::ostream &operator<<(std::ostream &os, Unique<ID, T> const &obj)
{
  return os << obj.value;
}


/*
 * __COUNTER__ is non-standard,
 * but is supported by: GCC, Clang, MSVS
 */
#define UQ(value) Unique<__COUNTER__, decltype(value)>{value}
