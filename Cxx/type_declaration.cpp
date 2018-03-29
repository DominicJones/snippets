template<class T> class Wrapper;

class Thing;
template<> class Wrapper<Thing> {};

int main()
{
  Wrapper<Thing> instance;
  return sizeof(instance);
}
