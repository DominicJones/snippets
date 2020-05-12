// from:
// https://stackoverflow.com/questions/13937873/how-can-i-prevent-a-variadic-constructor-from-being-preferred-to-the-copy-constr

#include <memory>
#include <type_traits>

template<typename T>
struct Foo
{
  Foo() = default;
  Foo(const Foo &) = default;

  template<typename Arg, typename... Args, typename Enable =
           std::enable_if_t<!(sizeof...(Args) == 0 && std::is_same<std::remove_reference_t<Arg>, Foo>::value)>
           >
    Foo(Arg&& arg, Args&&... args)
    : object(std::forward<Arg>(arg), std::forward<Args>(args)...) {}

  T object;
};


int main()
{
  auto x = Foo<std::shared_ptr<int>>(new int(42));
  decltype(x) x_cp{x};
  decltype(x) x_tmp(Foo<std::shared_ptr<int>>(new int));
}
