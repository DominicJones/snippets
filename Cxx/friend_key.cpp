template<typename... T> class FriendKey;

template<typename T>
class FriendKey<T>
{
private:
  friend T;

protected:
  FriendKey() {}
  FriendKey(FriendKey const &) {}
};

template<typename T, typename... Ts>
class FriendKey<T, Ts...> : FriendKey<T>, FriendKey<Ts...> {};


class Foo;

struct Bar
{
  void evaluate(Foo &foo);
};


struct Foo
{
  void special(FriendKey<Bar>) {}
  private: void secret() {}
};


void
Bar::evaluate(Foo &foo)
{
  foo.special({});
  // foo.secret();  // should not compile
}


int main()
{
  Foo foo;
  Bar{}.evaluate(foo);
}
