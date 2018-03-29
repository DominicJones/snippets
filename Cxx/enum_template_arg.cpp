// g++ without optimization removes the "if (false)" path

class Component
{
public:
  enum Option
    {
      None,
      All,
    };
};

class NoComponents
{
public:
  static int const option = Component::None;
};

class AllComponents
{
public:
  static int const option = Component::All;
};




class Derivative
{
};

template <class ComponentOption = AllComponents> class Derivative_
{
public:
  typedef ComponentOption Component;
};




int main()
{
  typedef Derivative_<> Derivative;

  int result = 0;

  if (Derivative::Component::option == Component::All)
    {
      int const array[37] = {37};
      result += array[0];
    }
  else
    {
      int const array[26] = {26};
      result += array[0];
    }

  return result;
}
