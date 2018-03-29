template <class B, class D>
class IsBaseOf
{
  static void constraints(D* p)
  {
    B* pb = p;
    pb = p;
  }

protected:
  IsBaseOf()
  {
    static void (*fp)(D*) = constraints;
  }
};


// B cannot be void:
template <class D>
class IsBaseOf<void, D>
{
  IsBaseOf() { char* p = (int*) 0; }
};


class Base {};
class Derived: public Base {};

template <class T>
class Container: public IsBaseOf<Base, T> {};

int main()
{
  Container<Derived> d;

  // causes compilation failure:
  // Container<char> b;
}
