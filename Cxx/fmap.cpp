#include <iostream>
#include <utility>


auto constexpr curry = [](auto f) {
  return [=](auto x) {
    return [=](auto y) {
      return f(x,y); }; }; };

auto constexpr terminal = [](auto x) {
  return [=](auto f) {
    return f(x); }; };

auto constexpr fmap = [](auto f) {
  return [=](auto x) {
    return terminal(f(x)); }; };


auto hello = [](auto s) { fprintf(s, "hello "); return s; };
auto world = [](auto s) { fprintf(s, "world "); return s; };
auto newline = [](auto s) { fprintf(s, "\n"); return s; };


auto constexpr xplusone = [](auto x){ return x+1; };
auto constexpr xplus = [](auto y){ return [=](auto x){ return x+y; }; };
auto constexpr plus = [](auto x, auto y){ return x+y; };


int main()
{
  std::cout << plus(2,3) << std::endl;
  std::cout << xplus(2)(3) << std::endl;

  std::cout << curry(plus)(2)(3) << std::endl;
  std::cout << terminal(2)(xplus)(3) << std::endl;

  terminal(stdout)(hello);

  terminal(stdout)
    (fmap(hello))
    (fmap(world))
    (fmap(newline));
}
