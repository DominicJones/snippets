#include <iostream>


void initialize(int &it)
{
  std::cout << "initialize" << std::endl;
  it = 0;
}

bool condition(int it, int size)
{
  std::cout << "condition" << std::endl;
  return it != size;
}


void increment(int &it)
{
  std::cout << "increment" << std::endl;
  ++it;
}


void statement()
{
  std::cout << "statement" << std::endl;
}


int main()
{
  int const size = 1;

  {
    int it;
    for (initialize(it); condition(it, size); increment(it))
    {
      statement();
    }
  }

  std::cout << std::endl;

  {
    int it;
    initialize(it);
    while (condition(it, size))
    {
      statement();
      increment(it);
    }
  }
}
