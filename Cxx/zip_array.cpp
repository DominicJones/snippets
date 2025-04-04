// based on:
//   https://github.com/EnricoFabiano85/cpp/blob/main/ZipVectorTemplates.cpp

#include <array>
#include <tuple>


template<int ...I>
struct Array : std::array<int, sizeof...(I)>
{
  constexpr Array() : std::array<int, sizeof...(I)>{I...} {}
};


template<typename V0, typename ...V>
struct Zip
{
  static consteval auto eval()
  {
    auto constexpr zip = [](auto v0, auto ...v) {
      auto result = std::array<int, v0.size()>{};
      result.fill(1);
      for (std::size_t i = 0; i != v0.size(); ++i) {
        result[i] *= v0[i] * (v[i] * ...);
      }
      return result;
    }(V0{}, V{}...);
    
    return [&]<auto ...I>(std::index_sequence<I...>) {
      return Array<zip[I]...>{};
    }(std::make_index_sequence<zip.size()>{});
  }

  using type = decltype(eval());
};


using Test = Zip<Array<1,1,2,3>, 
                 Array<1,2,3,4>, 
                 Array<1,3,4,5>>::type;
using Expected = Array<1,6,24,60>;

static_assert(std::same_as<Test, Expected>);


int main() {}
