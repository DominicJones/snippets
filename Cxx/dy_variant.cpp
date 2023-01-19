// taken from
// https://godbolt.org/z/crdd1xPcc
// by Denis Yaroshevskiy; https://github.com/DenisYaroshevskiy

#include <array>
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#define TOOLS_FWD(...) static_cast<decltype(__VA_ARGS__) &&>(__VA_ARGS__)

namespace tools {
namespace _instance_of {

template <template <typename ...> class, typename>
struct impl : std::false_type {};

template <template <typename ...> class What, typename ... Ts>
struct impl<What, What<Ts...>> : std::true_type {};

} // namespace _instance_of

template <typename T, template <typename ...> class What>
concept instance_of = _instance_of::impl<What, T>::value;

}  // namespace tools

namespace tools {
namespace _int_utils {

template <std::size_t max>
constexpr auto uint_at_least_impl() {
       if constexpr ( max <= std::numeric_limits<std::uint8_t>::max()  ) return std::uint8_t{};
  else if constexpr ( max <= std::numeric_limits<std::uint16_t>::max() ) return std::uint16_t{};
  else if constexpr ( max <= std::numeric_limits<std::uint32_t>::max() ) return std::uint32_t{};
  else                                                                   return std::uint64_t{};
}

}  // _int_utils

template <std::size_t max>
using uint_at_least_t = decltype(_int_utils::uint_at_least_impl<max>());

}  // namespace tools

// Kris Jusiak: The Nth Element: A Case Study https://youtu.be/LfOh0DwTP00
// https://github.com/willwray

namespace tools {
namespace _type_lists {

template <std::size_t n,
          typename T0 = void,
          typename T1 = void,
          typename T2 = void,
          typename T3 = void,
          typename T4 = void,
          typename T5 = void,
          typename T6 = void,
          typename T7 = void,
          typename T8 = void,
          typename T9 = void,
          typename ... Ts>
constexpr auto get_type_impl() {
  /**/ if constexpr ( n == 0 ) return std::type_identity<T0>{};
  else if constexpr ( n == 1 ) return std::type_identity<T1>{};
  else if constexpr ( n == 2 ) return std::type_identity<T2>{};
  else if constexpr ( n == 3 ) return std::type_identity<T3>{};
  else if constexpr ( n == 4 ) return std::type_identity<T4>{};
  else if constexpr ( n == 5 ) return std::type_identity<T5>{};
  else if constexpr ( n == 6 ) return std::type_identity<T6>{};
  else if constexpr ( n == 7 ) return std::type_identity<T7>{};
  else if constexpr ( n == 8 ) return std::type_identity<T8>{};
  else if constexpr ( n == 9 ) return std::type_identity<T9>{};
  else                         return get_type_impl<n - 10, Ts...>();
}

template <typename T, typename ...Ts>
constexpr std::size_t find_type_idx_impl() {
  std::size_t i = 0;
  for (bool x : { std::same_as<T, Ts> ...}) {
    if (x) break;
    ++i;
  }
  return i;
}

}  // namespace _type_lists

template <typename T, typename ...Ts>
  requires ( _type_lists::find_type_idx_impl<T, Ts...>() < sizeof...(Ts))
constexpr std::size_t find_type_idx_v = _type_lists::find_type_idx_impl<T, Ts...>();

template <typename T, typename ...Ts>
concept one_of = requires {
  { find_type_idx_v<T, Ts...> };
};

template <std::size_t n, typename ...Ts>
  requires (n < sizeof ...(Ts))
using get_type_t = typename decltype(_type_lists::get_type_impl<n, Ts...>())::type;

}  // namespace tools

namespace tools {

template <typename T, typename... Ts>
struct union_ {
  union {
    T head;
    union_<Ts...> tail;
  };

  constexpr union_() = default;

  constexpr union_(std::in_place_index_t<0u>, auto&& ...args):
    head (TOOLS_FWD(args)...) {}

  template <std::size_t i>
  constexpr union_(std::in_place_index_t<i> idx, auto&& ...args) :
    tail (std::in_place_index<i - 1>, TOOLS_FWD(args)...) {}
};

template <typename T>
struct union_<T> {
  T head;

  constexpr union_() = default;
  constexpr union_(std::in_place_index_t<0u>, auto&& ...args) : head (TOOLS_FWD(args)...) {}
};

template <typename>
struct union_size;

template <typename... Ts> struct union_size<union_<Ts...>>
  : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template <typename T>
constexpr std::size_t union_size_v = union_size<T>::value;

template <std::size_t i, typename Self>
constexpr auto&& get(Self&& self)
  requires instance_of<std::remove_cvref_t<Self>, union_> &&
          (i < union_size_v<std::remove_cvref_t<Self>>)
{
  if constexpr (i == 0) return TOOLS_FWD(self).head;
  else                  return get<i - 1>(TOOLS_FWD(self).tail);
}

template <std::size_t i, typename ...Ts>
constexpr void construct_at(union_<Ts...>& self, auto&&...args)
  requires requires { get<i>(self); } {
  std::construct_at(&self, std::in_place_index<i>, TOOLS_FWD(args)...);
}

template <std::size_t i, typename ...Ts>
constexpr void destroy_at(union_<Ts...>& self)
  requires requires { get<i>(self); } {
  std::destroy_at(&get<i>(self));
}

} // namespace tools

namespace tools {

template <std::size_t V>
struct c_idx {
  static constexpr auto value = V;
};

template <auto V>
constexpr c_idx<V> c_idx_v;

namespace v3 {
// ref: https://github.com/groundswellaudio/swl-variant
// https://godbolt.org/z/MGMdz7YvK

template <std::size_t size, typename Op>
struct switcher {
  Op op;

  using res_t = decltype(op(c_idx_v<0>));

  constexpr switcher(c_idx<size>, Op op) : op(op) {}

  template <std::size_t start>
  static constexpr res_t impl(Op _op, std::size_t i) {
    #define TOOLS_SWITCH_CASE(x) case start + x: if constexpr (size > start + x) return _op(c_idx_v<start + x>);
    #define TOOLS_SWITCH_CASE10(x) \
      TOOLS_SWITCH_CASE(10 * (x) + 0) \
      TOOLS_SWITCH_CASE(10 * (x) + 1) \
      TOOLS_SWITCH_CASE(10 * (x) + 2) \
      TOOLS_SWITCH_CASE(10 * (x) + 3) \
      TOOLS_SWITCH_CASE(10 * (x) + 4) \
      TOOLS_SWITCH_CASE(10 * (x) + 5) \
      TOOLS_SWITCH_CASE(10 * (x) + 6) \
      TOOLS_SWITCH_CASE(10 * (x) + 7) \
      TOOLS_SWITCH_CASE(10 * (x) + 8) \
      TOOLS_SWITCH_CASE(10 * (x) + 9)
    #define TOOLS_SWITCH_CASE100(x) \
      TOOLS_SWITCH_CASE10(10 * (x) + 0) \
      TOOLS_SWITCH_CASE10(10 * (x) + 1) \
      TOOLS_SWITCH_CASE10(10 * (x) + 2) \
      TOOLS_SWITCH_CASE10(10 * (x) + 3) \
      TOOLS_SWITCH_CASE10(10 * (x) + 4) \
      TOOLS_SWITCH_CASE10(10 * (x) + 5) \
      TOOLS_SWITCH_CASE10(10 * (x) + 6) \
      TOOLS_SWITCH_CASE10(10 * (x) + 7) \
      TOOLS_SWITCH_CASE10(10 * (x) + 8) \
      TOOLS_SWITCH_CASE10(10 * (x) + 9)

    switch (i) {
      TOOLS_SWITCH_CASE100(0)
      TOOLS_SWITCH_CASE100(1)
      TOOLS_SWITCH_CASE100(2)
      TOOLS_SWITCH_CASE100(3)
      TOOLS_SWITCH_CASE100(4)
      default: if constexpr (size >= start + 500) return impl<start + 500>(_op, i);
               else                               __builtin_unreachable();
    }

    #undef TOOLS_SWITCH_CASE100
    #undef TOOLS_SWITCH_CASE10
    #undef TOOLS_SWITCH_CASE
  }

  constexpr res_t operator()(std::size_t i) const
  {
    return impl<0>(op, i);
  }
};

} // namespace v3

}  // namespace tools

namespace tools {

template <typename T>
concept error_tag_type = requires { typename T::is_error; };

template <typename ...Ts>
concept any_errors = ( error_tag_type<Ts> || ... );

template <typename ...Ts>
constexpr std::size_t first_error_idx() {
  std::array tests { error_tag_type<Ts>... };
  return static_cast<std::size_t>(std::ranges::find(tests, true) - tests.begin());
}

template <typename ...Ts>
using first_error_t = get_type_t<first_error_idx<Ts...>(), Ts...>;

}  // namespace tools

namespace tools {

template <typename ...Ts>
struct no_common_type {
  using is_error = void;
};

template <typename ...Ts>
auto common_type_impl() {
  /**/ if constexpr (( error_tag_type<Ts> || ... ) ) return std::type_identity<first_error_t<Ts...>>{};
  else if constexpr ( requires { typename std::common_type_t<Ts...>; } ) return std::common_type<Ts...>{};
  else return std::type_identity<no_common_type<Ts...>>{};
}

template <typename ...Ts>
using common_type_t = typename decltype(common_type_impl<Ts...>())::type;

}  // namespace tools

namespace tools {

template <std::size_t ... dims>
struct c_array_math {
  using mutli_idx = std::array<std::size_t, sizeof...(dims)>;

  static constexpr mutli_idx pows = [] {
    std::array dims_a = {dims...};
    std::array res = dims_a;
    for (std::size_t running = 1, i = dims_a.size(); i > 0; --i) {
      res[i - 1] = running;
      running *= dims_a[i - 1];
    }
    return res;
  }();

  static constexpr mutli_idx to_multi_idx(std::size_t idx) {
      auto res = pows;
      for (std::size_t i = 0; i != pows.size(); ++i) {
        res[i] = idx / pows[i];
        idx %= pows[i];
      }
      return res;
  }

  static constexpr std::size_t to_linear_idx(mutli_idx idx) {
    return std::inner_product(pows.begin(), pows.end(), idx.begin(), 0u);
  }


  template <std::size_t idx>
  static constexpr auto to_mutli_idx_sequence() {
    constexpr auto arr = to_multi_idx(idx);

    return [&]<std::size_t ...is>(std::index_sequence<is...>) {
      return std::index_sequence<arr[is]...>{};
    }(std::make_index_sequence<sizeof...(dims)>{});
  }

  template <std::size_t i>
  using multi_idx_sequence = decltype(to_mutli_idx_sequence<i>());
};

}  // namespace tools

namespace tools::detail {

template <typename ...Ts>
struct not_invocable {
  using is_error = void;
};

template <typename Vis, typename ...Ts>
struct one_case_result_type {
  using type = not_invocable<Vis, Ts...>;
  static constexpr bool is_noexcept = false;
};

template <typename Vis, typename ...Ts>
  requires std::invocable<Vis, Ts...>
struct one_case_result_type<Vis, Ts...>   {
  using type = std::invoke_result_t<Vis, Ts...>;
  static constexpr bool is_noexcept =
    noexcept(std::declval<Vis>()(std::declval<Ts>()...));
};

template <std::size_t case_, typename Vis, typename ...Vs>
auto one_case_result_impl(c_idx<case_>, Vis&& vis, Vs&& ... vs) {
  using math = c_array_math<std::remove_cvref_t<Vs>::size...>;
  using seq = typename math::template multi_idx_sequence<case_>;

  return [&]<std::size_t ... is>(std::index_sequence<is...>) {
    return one_case_result_type<Vis, decltype(get<is>(TOOLS_FWD(vs).data)) ...>{};
  }(seq{});
};

template <std::size_t case_, typename Vis, typename ...Vs>
using one_case_res_t = typename decltype(one_case_result_impl(c_idx<case_>{}, std::declval<Vis>(), std::declval<Vs>()...))::type;

template <std::size_t case_, typename Vis, typename ...Vs>
constexpr bool one_case_noexcept_v = decltype(one_case_result_impl(c_idx<case_>{}, std::declval<Vis>(), std::declval<Vs>()...))::is_noexcept;

}  // namespace tools::detail

namespace tools::detail
{

template <typename Vis, typename... Vs, std::size_t... i>
auto visit_result_impl(std::index_sequence<i...>) {
  using res_t = tools::common_type_t<one_case_res_t<i, Vis, Vs...> ...>;
  if constexpr ( error_tag_type<res_t> ) return res_t{};
  else return std::type_identity<res_t>{};
}

template <typename Vis, typename... Vs>
using visit_result_t = typename decltype(visit_result_impl<Vis, Vs...>(
    std::make_index_sequence<(std::remove_cvref_t<Vs>::size * ... )>{})
  )::type;

template <typename Vis, typename... Vs>
constexpr bool noexcept_visit_v = []<std::size_t ... i>(std::index_sequence<i...>) {
  return (one_case_noexcept_v<i, Vis, Vs...> && ...);
}(std::make_index_sequence<(std::remove_cvref_t<Vs>::size * ... )>{});

} // namespace tools::detail


namespace tools::detail {

template <typename ... Ts>
struct variant_data {
  using index_type = uint_at_least_t<sizeof...(Ts)>;
  static constexpr std::size_t size = sizeof...(Ts);

  union_<Ts...> data;
  index_type idx;
};

template <typename Vis, typename ...Vs>
constexpr auto visit(Vis&& vis, Vs&& ... vs)
  noexcept ( noexcept_visit_v<Vis, Vs...> ) -> visit_result_t<Vis, Vs...> {
  using math = c_array_math<std::remove_cvref_t<Vs>::size...>;
  using res = visit_result_t<Vis, Vs...>;

  v3::switcher switcher {
    c_idx_v<(std::remove_cvref_t<Vs>::size * ...)>, [&]<std::size_t case_>(c_idx<case_>) {
      using seq = typename math::template multi_idx_sequence<case_>;

      return [&]<std::size_t ... is>(std::index_sequence<is...>) -> res {
        return TOOLS_FWD(vis)(get<is>(TOOLS_FWD(vs).data) ...);
      }(seq{});
  }};

  auto idx = math::to_linear_idx({vs.idx ...});

  if constexpr ( !std::same_as<res, void> ) {
    return switcher(idx);
  } else {
    switcher(idx);
  }
}

}  // namespace tools::detail

namespace visit_result_test {

using namespace tools::detail;

using V1 = tools::detail::variant_data<char, short>;
using V2 = tools::detail::variant_data<int, double>;
using V3 = tools::detail::variant_data<std::string, double>;

auto min_l = [](auto a, auto b) -> decltype( b < a ? b : a ) {
  return b < a ? b : a;
};

static_assert(std::same_as<decltype(
  tools::detail::visit(min_l, std::declval<V1>(), std::declval<V2>())),
  double>);

#if 0
void foo(V1& v1, V3& v3) {
  tools::detail::visit(min_l, v1, v3);
}
#endif

}  // namespace visit_result_test

namespace tools {
namespace _variant {

constexpr auto&& get_data(auto&& self) {
  return TOOLS_FWD(self).data;
}

}  // namespace _variant

template <typename ...Ts>
class variant {
  detail::variant_data<Ts...> data;
  friend constexpr auto&& _variant::get_data(auto&& self);
 public:
  template <one_of<Ts...> T>
  constexpr explicit variant(T x) {
    tools::construct_at<tools::find_type_idx_v<T, Ts...>>(data.data, x);
    data.idx = tools::find_type_idx_v<T, Ts...>;
  }

  constexpr variant(const variant& _x) noexcept(
      (std::is_nothrow_copy_constructible_v<Ts> && ...)
  ) {
    detail::visit([&]<typename T>(const T& x) mutable {
      tools::construct_at<tools::find_type_idx_v<T, Ts...>>(data.data, x);
    }, _x.data);
    data.idx = _x.data.idx;
  }

  constexpr ~variant() {
    detail::visit([]<typename T>(T& self) {
      std::destroy_at(&self);
    }, data);
  }
};

template <typename Vis, typename ...Vs>
  requires ( instance_of<std::remove_cvref_t<Vs>, tools::variant> && ... )
constexpr auto visit(Vis&& vis, Vs&& ... vs)
  noexcept ( noexcept(detail::visit(TOOLS_FWD(vis), _variant::get_data(TOOLS_FWD(vs)) ...)) ) ->
  detail::visit_result_t<Vis, decltype(_variant::get_data(TOOLS_FWD(vs)))...>
{
  return detail::visit(TOOLS_FWD(vis), _variant::get_data(TOOLS_FWD(vs)) ...);
}

}  // namespace tools














#include <exception>
#include <stdexcept>
#include <vector>

namespace
{
namespace instance_of_test {
static_assert(tools::instance_of<std::vector<int>, std::vector>);
static_assert(!tools::instance_of<int, std::vector>);
static_assert(!tools::instance_of<std::vector<int>&, std::vector>);
} // namespace instance_of_test

namespace union_test {

using us = tools::union_<int, char, double>;

static_assert(std::same_as<int const& , decltype(get<0>(std::declval<us const& >()))>);
static_assert(std::same_as<int&,        decltype(get<0>(std::declval<us&       >()))>);
static_assert(std::same_as<int&&,       decltype(get<0>(std::declval<us&&      >()))>);
static_assert(std::same_as<int const&&, decltype(get<0>(std::declval<us const&&>()))>);

static_assert(std::same_as<char const& , decltype(get<1>(std::declval<us const& >()))>);
static_assert(std::same_as<char&,        decltype(get<1>(std::declval<us&       >()))>);
static_assert(std::same_as<char&&,       decltype(get<1>(std::declval<us&&      >()))>);
static_assert(std::same_as<char const&&, decltype(get<1>(std::declval<us const&&>()))>);

static_assert(std::same_as<double const& , decltype(get<2>(std::declval<us const& >()))>);
static_assert(std::same_as<double&,        decltype(get<2>(std::declval<us&       >()))>);
static_assert(std::same_as<double&&,       decltype(get<2>(std::declval<us&&      >()))>);
static_assert(std::same_as<double const&&, decltype(get<2>(std::declval<us const&&>()))>);

constexpr bool union_runtime() {
  tools::union_<int, char, double> x;
  tools::construct_at<2>(x, 1.0);
  if (get<2>(x) != 1.0) return false;
  tools::destroy_at<2>(x);
  tools::construct_at<1>(x, 'a');
  if (get<1>(x) != 'a') return false;
  tools::destroy_at<1>(x);
  return true;
}

static_assert(union_runtime());

}  // namespace union_test

namespace switch_test {

template <std::size_t a, std::size_t b>
concept test = a == b;

constexpr auto identity = []<std::size_t i>(tools::c_idx<i>) mutable { return i; };

constexpr auto v3_identity = tools::v3::switcher{tools::c_idx_v<2000>, identity};
static_assert(999u  == v3_identity(999u));
static_assert(1u    == v3_identity(1u));
static_assert(1500u == v3_identity(1500u));

void switch_runtime_test() {
  for (auto i = 0u; i != 2000u; ++i) {
   if(v3_identity(i) != i) throw std::runtime_error("v3");
  }
}

}  // namespace switch_test

namespace int_util_test {

static_assert(std::same_as<std::uint8_t,  tools::uint_at_least_t<100>>);
static_assert(std::same_as<std::uint8_t,  tools::uint_at_least_t<255>>);
static_assert(std::same_as<std::uint16_t, tools::uint_at_least_t<256>>);
static_assert(std::same_as<std::uint32_t, tools::uint_at_least_t<std::numeric_limits<std::uint16_t>::max() + 1u>>);
static_assert(std::same_as<std::uint64_t, tools::uint_at_least_t<5'000'000'000>>);

} // namespace int_util_test

namespace c_array_math_test {

constexpr bool test() {
  using math = tools::c_array_math<3, 4, 2>;
  int arr[3][4][2] = {};

  std::size_t linear = 0u;

  for (std::size_t i = 0; i != 3; ++i) {
    for (std::size_t j = 0; j != 4; ++j) {
      for (std::size_t k = 0; k != 2; ++k) {
        auto multi = std::array{i, j, k};
        if (math::to_multi_idx(linear) != multi) return false;
        if (math::to_linear_idx(multi) != linear) return false;
        ++linear;
      }
    }
  }

  return true;
}

static_assert(test());

}  // namespace c_array_math_test

namespace get_type_test {

using namespace tools;

template <std::size_t i>
struct ith {};

template <std::size_t i>
using call_get_t =
  get_type_t<i,
    ith<0>,  ith<1>,  ith<2>,  ith<3>,  ith<4>,
    ith<5>,  ith<6>,  ith<7>,  ith<8>,  ith<9>,
    ith<10>, ith<11>, ith<12>, ith<13>, ith<14>>;

template <std::size_t i>
concept can_call_get = requires {
  { std::declval<call_get_t<i>>() };
};

static_assert(std::same_as<call_get_t<0>, ith<0>>);
static_assert(std::same_as<call_get_t<1>, ith<1>>);
static_assert(std::same_as<call_get_t<2>, ith<2>>);
static_assert(std::same_as<call_get_t<3>, ith<3>>);
static_assert(std::same_as<call_get_t<4>, ith<4>>);

static_assert(std::same_as<call_get_t<5>, ith<5>>);
static_assert(std::same_as<call_get_t<6>, ith<6>>);
static_assert(std::same_as<call_get_t<7>, ith<7>>);
static_assert(std::same_as<call_get_t<8>, ith<8>>);
static_assert(std::same_as<call_get_t<9>, ith<9>>);

static_assert(std::same_as<call_get_t<10>, ith<10>>);
static_assert(std::same_as<call_get_t<11>, ith<11>>);
static_assert(std::same_as<call_get_t<12>, ith<12>>);
static_assert(std::same_as<call_get_t<13>, ith<13>>);
static_assert(std::same_as<call_get_t<14>, ith<14>>);

static_assert(find_type_idx_v<ith<2>, ith<0>, ith<1>, ith<2>> == 2);
static_assert(find_type_idx_v<ith<1>, ith<0>, ith<1>, ith<2>> == 1);
static_assert(one_of<int, char, int, double>);
static_assert(!one_of<int, char, double>);


static_assert(can_call_get<11>);
static_assert(!can_call_get<15>);
static_assert(!can_call_get<16>);

static_assert(
  std::same_as<
    get_type_t<0, int, char, double>,
    int
  >
);


static_assert(
  std::same_as<
    get_type_t<1, int, char, double>,
    char
  >
);

static_assert(
  std::same_as<
    get_type_t<2, int, char, double>,
    double
  >
);


}  // namespace get_type_test

namespace first_error_test {

using namespace tools;
using namespace tools::detail;

struct err_t {
  using is_error = void;
};

static_assert(std::same_as<first_error_t<err_t>, err_t>);
static_assert(std::same_as<first_error_t<int, err_t>, err_t>);
static_assert(std::same_as<first_error_t<int, int, err_t>, err_t>);
static_assert(std::same_as<first_error_t<int, err_t, int, err_t>, err_t>);

static_assert(std::same_as<
  first_error_t<not_invocable<int, int>>, not_invocable<int, int>>);

} // namespace first_error_test

namespace common_type_test {

using namespace tools;
using namespace tools::detail;

static_assert(std::same_as<common_type_t<>, no_common_type<>>);
static_assert(std::same_as<common_type_t<int, double>, double>);
static_assert(std::same_as<common_type_t<double, int, int>, double>);
static_assert(std::same_as<common_type_t<std::string, int, int>,
              no_common_type<std::string, int, int>>);

static_assert(std::same_as<common_type_t<not_invocable<int, int>, int>,
                           not_invocable<int, int>>);

static_assert(std::same_as<common_type_t<not_invocable<int, int>, not_invocable<int, double>>,
                           not_invocable<int, int>>);

}  // namespace common_type_test

namespace one_case_result_test {

struct A {};
struct B {};
struct C {};
struct D {};

using V1 = tools::detail::variant_data<A, B>;
using V2 = tools::detail::variant_data<C, D>;


struct Visit {
  A&& operator()(A&&, C&&) const noexcept;
  D&& operator()(A&&, D&&) const;
  B&& operator()(B&&, C&&) const;
  D&& operator()(B&&, D&&) const noexcept;
};

using namespace tools::detail;

static_assert(
  std::same_as<one_case_res_t<0, Visit, V1&&, V2&&>, A&&>
);

static_assert(one_case_noexcept_v<0, Visit, V1&&, V2&&>);

static_assert(
  std::same_as<one_case_res_t<1, Visit, V1&&, V2&&>, D&&>
);

static_assert(!one_case_noexcept_v<1, Visit, V1&&, V2&&>);

static_assert(
  std::same_as<one_case_res_t<2, Visit, V1&&, V2&&>, B&&>
);

static_assert(
  std::same_as<one_case_res_t<3, Visit, V1&&, V2&&>, D&&>
);

static_assert(
  std::same_as<one_case_res_t<0, Visit, V1 const&, V2&&>,
  not_invocable<Visit, A const &, C&&>>
);

}  // namespace one_case_result_test

namespace visit_result_test {

using namespace tools::detail;

using V1 = tools::detail::variant_data<char, short>;
using V2 = tools::detail::variant_data<int, double>;
using V3 = tools::detail::variant_data<std::string, double>;

auto min_l = [](auto a, auto b) -> decltype( b < a ? b : a ) {
  return b < a ? b : a;
};

static_assert(std::same_as<decltype(
  tools::detail::visit(min_l, std::declval<V1>(), std::declval<V2>())),
  double>);

#if 0
void foo(V1& v1, V3& v3) {
  tools::detail::visit(min_l, v1, v3);
}
#endif

}  // namespace visit_result_test

namespace variant_test {

constexpr bool foo() {
  tools::variant<int, char, double> v1('a');
  tools::variant<float, float> v2(0.1f);
  auto v3 = v1;

  return visit([](auto x, auto y) {
    return std::same_as<decltype(x), char> &&
           std::same_as<decltype(y), float> &&
           (x == 'a') &&
           (y == 0.1f);
  }, v3, v2);
}

static_assert(foo());

}  // namespace variant_test

#if 0
namespace variant_error {

using V1 = tools::variant<char, short>;
using V2 = tools::variant<int, double>;
using V3 = tools::variant<std::string, double>;

auto min_l = [](auto a, auto b) -> decltype( b < a ? b : a ) {
  return b < a ? b : a;
};

static_assert(std::same_as<decltype(
  tools::visit(min_l, std::declval<V1>(), std::declval<V2>())),
  double>);

void foo(V1& v1, V3& v3) {
  tools::visit(min_l, v1, v3);
}

}  // namespace  variant_error
#endif

} // namespace

