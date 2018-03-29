#include <iostream>
#include <utility>

// code lowering of lambda 'xplusone'
struct xplusone_t {
  template<typename T>
  auto operator()(T const &x) const -> decltype(x + 1) { return x+1; }
};

auto xplusone = [](auto x){ return x+1; };

auto xplus = [](auto y){ return [=](auto x){ return x+y; }; };

auto plus = [](auto a, auto b){ return a+b; };

auto curry = [](auto f){
  return [=](auto x) {
    return [=](auto y) {
      return f(x,y); }; }; };

auto terminal = [](auto term) {
  return [=](auto func) {
    return func(term); }; };

auto hello = [](auto s) { fprintf(s, "hello "); return s; };

auto fmap = [](auto f) {
  return [=](auto t) {
    return terminal(f(t)); }; };

auto world = [](auto s) { fprintf(s, "world "); return s; };
auto newline = [](auto s) { fprintf(s, "\n"); return s; };


int main()
{
  std::cout << xplus(2)(3) << std::endl;

  std::cout << curry(plus)(2)(3) << std::endl;

  terminal(stdout)(hello);

  terminal(stdout)
    (fmap(hello))
    (fmap(world))
    (fmap(newline));
}
