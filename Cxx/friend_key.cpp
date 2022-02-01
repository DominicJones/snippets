template<typename... T> class FriendKey;

template<typename T>
class FriendKey<T>
{
protected:
  FriendKey() {}
  FriendKey(FriendKey const &) {}
private:
  friend T;
};

template<typename T, typename... Ts>
class FriendKey<T, Ts...> : FriendKey<T>, FriendKey<Ts...> {};


class Foo;
class Bar;
class Baz;

struct Foo
{
  void special(FriendKey<Bar, Baz>) const {}
  private: void secret() const {}
};

struct Bar
{
  void evaluate(Foo const &foo)
  {
    foo.special({});
    // foo.secret();  // should not compile
  }
};

struct Baz
{
  void evaluate(Foo const &foo)
  {
    foo.special({});
    // foo.secret();  // should not compile
  }
};


int main()
{
  Bar{}.evaluate(Foo{});
  Baz{}.evaluate(Foo{});
}
