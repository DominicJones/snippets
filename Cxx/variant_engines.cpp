#include <iostream>
#include <variant>
#include <vector>


template<typename T>
struct UnreachableEngine final
{
  T operator[](int const &) const { return T{0}; }
};


template<typename T>
class ValueEngine final
{
public:
  ValueEngine(T const &state) : _state(state) {}
  T operator[](int const &) const { return _state; }

private:
  T const _state;
};


template<typename T>
class FieldEngine final
{
public:
  FieldEngine(std::vector<T> const &state) : _state(state) {}
  T operator[](int const &i) const { return _state[i]; }

private:
  std::vector<T> const &_state;
};


template<typename T>
class DynamicEngine final
{
public:
  DynamicEngine(T const &state)
  {
    _engine.template emplace<ValueEngine<T> >(state);
  }

  DynamicEngine(std::vector<T> const &state)
  {
    _engine.template emplace<FieldEngine<T> >(state);
  }

  T operator[](int const &i) const
  {
    return std::visit([&i](auto &&e){ return e[i]; }, _engine);
  }

private:
  using Engine = std::variant<UnreachableEngine<T>, ValueEngine<T>, FieldEngine<T> >;
  Engine _engine;
};


int main()
{
  int value = 3;
  std::vector<int> field(1, 5);

  // DynamicEngine<int> engine{value};
  DynamicEngine<int> engine{field};

  std::cout << engine[0] << std::endl;
}
