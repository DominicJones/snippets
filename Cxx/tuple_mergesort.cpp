// taken from:
// http://stackoverflow.com/a/34235074

#include <type_traits>

template <class... T>
struct TypeList {
  static constexpr const std::size_t size = sizeof...(T);
};

/***
 * Concat metafunction
 */

template <typename A, typename B>
struct Concat;

template <class... As, class... Bs>
struct Concat<TypeList<As...>, TypeList<Bs...>> {
  typedef TypeList<As..., Bs...> type;
};

template <typename A, typename B>
using Concat_t = typename Concat<A, B>::type;

/***
 * Split metafunction
 */

template <int i, typename TL>
struct Split;

template <int k, typename... TL>
struct Split<k, TypeList<TL...>> {
private:
  typedef Split<k / 2, TypeList<TL...>> FirstSplit;
  typedef Split<k - k / 2, typename FirstSplit::R> SecondSplit;

public:
  typedef Concat_t<typename FirstSplit::L, typename SecondSplit::L> L;
  typedef typename SecondSplit::R R;
};

template <typename T, typename... TL>
struct Split<0, TypeList<T, TL...>> {
  typedef TypeList<> L;
  typedef TypeList<T, TL...> R;
};

template <typename T, typename... TL>
struct Split<1, TypeList<T, TL...>> {
  typedef TypeList<T> L;
  typedef TypeList<TL...> R;
};

template <int k>
struct Split<k, TypeList<>> {
  typedef TypeList<> L;
  typedef TypeList<> R;
};

// Metafunction Subdivide: Split a typelist into two roughly equal typelists
template <typename TL>
struct Subdivide : Split<TL::size / 2, TL> {};

/***
 * Ordered tuple
 */

template <int X>
using int_t = std::integral_constant<int, X>;

template <class... T>
struct Ordered_List : TypeList<T...> {};

template <class... As, class... Bs>
struct Concat<Ordered_List<As...>, Ordered_List<Bs...>> {
  typedef Ordered_List<As..., Bs...> type;
};

/***
 * Merge metafunction
 */

template <typename A, typename B>
struct Merge;

template <typename B>
struct Merge<Ordered_List<>, B> {
  typedef B type;
};

template <int a, class... As>
struct Merge<Ordered_List<int_t<a>, As...>, Ordered_List<>> {
  typedef Ordered_List<int_t<a>, As...> type;
};

template <int a, class... As, int b, class... Bs>
struct Merge<Ordered_List<int_t<a>, As...>, Ordered_List<int_t<b>, Bs...>> {
  typedef Ordered_List<int_t<a>, As...> A;
  typedef Ordered_List<int_t<b>, Bs...> B;

  typedef typename std::conditional<a <= b,
                                    Concat_t<Ordered_List<int_t<a>>, typename Merge<Ordered_List<As...>, B>::type>,
                                    Concat_t<Ordered_List<int_t<b>>, typename Merge<A, Ordered_List<Bs...>>::type>
                                   >::type type;
};

template <typename A, typename B>
using Merge_t = typename Merge<A, B>::type;

/***
 * Mergesort metafunction
 */

template <typename TL>
struct MergeSort;

// Boilerplate base-cases
template <>
struct MergeSort<TypeList<>> {
  typedef Ordered_List<> type;
};

template <int X>
struct MergeSort<TypeList<int_t<X>>> {
  typedef Ordered_List<int_t<X>> type;
};

// Inductive step
template <int X, class... Xs>
struct MergeSort<TypeList<int_t<X>, Xs...>> {
  typedef TypeList<int_t<X>, Xs...> input_t;
  typedef Subdivide<input_t> subdivide_t;
  typedef typename MergeSort<typename subdivide_t::L>::type left_sort_t;
  typedef typename MergeSort<typename subdivide_t::R>::type right_sort_t;
  typedef Merge_t<left_sort_t, right_sort_t> type;
};

template <typename T>
using MergeSort_t = typename MergeSort<T>::type;

/***
 * Make ordered tuple impl
 */

#include <tuple>

template <typename T>
struct MakeStdTuple;

template <class... Ts>
struct MakeStdTuple<Ordered_List<Ts...>> {
  typedef std::tuple<Ts...> type;
};

template <typename T>
using MakeStdTuple_t = typename MakeStdTuple<T>::type;

template <class... T>
constexpr MakeStdTuple_t<MergeSort_t<TypeList<T...>>> make_ordered_tuple(T&&...) {
  return {};
}

static_assert(std::is_same<Ordered_List<int_t<1>, int_t<2>, int_t<3>>,
                           MergeSort_t<TypeList<int_t<1>, int_t<2>, int_t<3>>>>::value, "Failed a unit test");

static_assert(std::is_same<Ordered_List<int_t<1>, int_t<2>, int_t<3>>,
                           MergeSort_t<TypeList<int_t<2>, int_t<1>, int_t<3>>>>::value, "Failed a unit test");

static_assert(std::is_same<Ordered_List<int_t<1>, int_t<2>, int_t<3>>,
                           MergeSort_t<TypeList<int_t<3>, int_t<2>, int_t<1>>>>::value, "Failed a unit test");

int main() {}
