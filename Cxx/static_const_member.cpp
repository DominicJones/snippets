template <int N> class Container
{
public:
  static const int Size = N;
  static int size() { return N; }
};

int main()
{
  typedef Container<5> SmallContainer;
  SmallContainer c1;
  Container<SmallContainer::Size> c2;

  // The following is not possible because
  // Container::size() is not a const-expr:
  // Container<SmallContainer::size()> c3;
}
