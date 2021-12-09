#include <utility>

struct Cell
{
  Cell(int){};
};

template<typename T>
T const &danglingReferenceOf()
{
  T const * const ptr = nullptr;
  return *ptr;
}

void eval(Cell const &) {}

int main()
{
  eval(danglingReferenceOf<Cell>());
  // eval(std::declval<Cell>()); // error
}
