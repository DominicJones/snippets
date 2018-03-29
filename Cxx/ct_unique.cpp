template<class T, std::size_t> struct unique_t
{
  T const value;
};

#define UNIQ(value) unique_t<decltype(value), __COUNTER__>{value}

int main()
{
  auto x = UNIQ(3);
  std::cout << x.value << std::endl;
}
