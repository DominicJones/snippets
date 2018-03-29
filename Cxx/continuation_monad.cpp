#include <iostream>
#include <utility>


auto tuple = [](auto ...x) {
  return [=](auto access) { return access(x...); };
};


auto head = [](auto xs) {
  return xs([](auto first, auto ...rest) { return first; });
};

auto tail = [](auto xs) {
  return xs([](auto first, auto ...rest) { return tuple(rest...); });
};

auto length = [](auto xs) {
  return xs([](auto ...x) { return sizeof...(x); });
};


template<std::size_t n, typename = std::make_index_sequence<n> >
class get_impl;

template<std::size_t n, std::size_t ...ignore>
class get_impl<n, std::index_sequence<ignore...> >
{
public:
  static auto at(auto xs) {
    return xs([](auto ...x) { return at_impl(&x...); });
  }

private:
  template<typename T>
  static T at_impl(decltype((void*)ignore)..., T* x, ...) { return *x; }
};

template<std::size_t n, typename Ts>
auto get(Ts xs)
{
  return get_impl<n>::at(xs);
}


int main()
{
  auto t = tuple(12, 'x', 3.4);
  std::cout << head(t) << std::endl;
  std::cout << head(tail(t)) << std::endl;
  std::cout << length(t) << std::endl;
  std::cout << get<0>(t) << std::endl;
  std::cout << get<1>(t) << std::endl;
  std::cout << get<2>(t) << std::endl;
}
