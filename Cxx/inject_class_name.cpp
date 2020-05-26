// C++20

#include <iostream>
#include <string>

#define quote(x) #x


template<unsigned N>
class FixedString
{
public:
  constexpr FixedString(char const * s)
  {
    for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
  }

  constexpr operator char const *() const { return buf; }

private:
  char buf[N + 1] {};
};

template<unsigned N> FixedString(char const (&)[N]) -> FixedString<N - 1>;


template<FixedString s>
struct ClassName
{
  static auto constexpr className() { return std::string(s); }
};


struct TestABC
  : ClassName<quote(TestABC)>
{};


int main()
{
  std::cout << TestABC::className() << std::endl;
}
