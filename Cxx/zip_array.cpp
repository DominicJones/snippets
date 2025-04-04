// from:
//   https://github.com/EnricoFabiano85/cpp/blob/main/ZipVectorTemplates.cpp

#include <array>
#include <tuple>


template<int ...I>
struct Array : std::array<int, sizeof...(I)>
{
  constexpr Array() : std::array<int, sizeof...(I)>{I...} {}
};


template<typename ...V>
struct Zip
{
  static consteval auto eval()
  {
    auto constexpr size{sizeof...(V)};

    auto constexpr zip = [](auto ...v) {
      auto result = std::array<int, size>{};
      result.fill(1);
      for (std::size_t I = 0; I != size; ++I) {
        result[I] *= (v[I] * ...);
      }
      return result;
    }(V{}...);
    
    return [&]<auto ...I>(std::index_sequence<I...>) {
      return Array<zip[I]...>{};
    }(std::make_index_sequence<size>{});
  }

  using type = decltype(eval());
};


using Test = Zip<Array<1,2,3>, 
                 Array<2,3,4>, 
                 Array<3,4,5>>::type;
using Expected = Array<6,24,60>;

static_assert(std::same_as<Test, Expected>);


int main() {}
