#include <type_traits>

struct ShadowBlocker;

#define ShadowBlockerLock() \
  static_assert(!std::is_destructible<ShadowBlocker>::value, ""); struct ShadowBlocker {};

int main()
{
  ShadowBlockerLock();
  {
    ShadowBlockerLock();
  }
}
