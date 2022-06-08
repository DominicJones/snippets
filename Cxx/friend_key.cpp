#include <utility>

struct FriendKeyBackdoor;

template<typename... T> class FriendKey;

template<typename T, typename... Ts>
class FriendKey<T, Ts...> : FriendKey<T>, FriendKey<Ts...> {};

template<typename T>
class FriendKey<T>
{
  friend FriendKeyBackdoor;
  friend T;
  FriendKey() {}
  FriendKey(FriendKey const &) {}
};

struct FriendKeyBackdoor
{
  template<typename MemFn, typename Object, typename... Args>
  static auto invoke(MemFn memFn, Object &&object, Args &&...args)
    -> decltype((object.*memFn)(std::forward<Args>(args)..., {}))
  {
    return (object.*memFn)(std::forward<Args>(args)..., {});
  }
};


class Foo;
class Bar;
class Baz;

struct Foo
{
  void special(FriendKey<Bar>) const {}
  private: void secret() const {}
};

struct Bar
{
  void evaluate(Foo const &foo)
  {
    foo.special({});
    // foo.secret();    // does not compile
  }
};

struct Baz
{
  void evaluate(Foo const &foo)
  {
    FriendKeyBackdoor::invoke(&Foo::special, foo);
    // foo.special({}); // does not compile
    // foo.secret();    // does not compile
  }
};


int main()
{
  Bar{}.evaluate(Foo{});
  Baz{}.evaluate(Foo{});
}
