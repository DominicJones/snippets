#include <iostream>
#include <memory>
#include <iostream>


// preamble
template<typename T> struct type_
{
  using type = T;
  T value;
};

template <class... T> struct list {};


// set (unique list)
template<class... Ts>
struct make_set;

template <class... T>
struct set_impl
{
  std::tuple<T...> values;

  template<class K, class = decltype(static_cast<type_<K>*>(static_cast<make_set<T...>*>(nullptr)))>
  static set_impl insert_t(type_<K> v);

  template<class K>
  static set_impl<T..., typename K::type> insert_t(K v);

  template<class K, class = decltype(static_cast<type_<K>*>(static_cast<make_set<T...>*>(nullptr)))>
  set_impl insert(type_<K> v)
  {
    return set_impl{this->values};
  }

  template<class K>
  set_impl<T..., typename K::type> insert(K v)
  {
    auto const v2 = std::make_tuple(v.value);
    auto const vv = std::tuple_cat(values, v2);
    return set_impl<T..., typename K::type>{vv};
  }
};

template<class... Ts>
struct make_set : type_<Ts>...
{
  using type = set_impl<Ts...>;
};

template<class... Ts>
using set = typename make_set<Ts...>::type;


// insert function
template <class C, class T>
struct insert_impl
{
  using type = decltype(C::insert_t(type_<T>{}));
};

template<class L, class T>
using insert = typename insert_impl<L, T>::type;


// join two sets
template<std::size_t idx, typename L, typename... Ts>
struct join_impl;

template<std::size_t idx, typename L>
struct join_impl<idx, L>
{
  using type = L;

  template<class M>
  static type apply(L l, M m)
  {
    return type{l};
  }
};

template<std::size_t idx, typename L, typename T1, typename... Ts>
struct join_impl<idx, L, T1, Ts...>
  : join_impl<idx + 1, L, Ts...>
{
  using base_t = join_impl<idx + 1, L, Ts...>;
  using type = insert<typename base_t::type, T1>;

  template<class M>
  static type apply(L l, M m)
  {
    T1 v = std::get<idx>(m);
    return type{base_t::apply(l, m).insert(type_<T1>{v})};
  }
};

template<typename L, typename... Ts>
struct join
  : join_impl<0, L, Ts...>
{
  using base_t = join_impl<0, L, Ts...>;
};

template<typename L>
struct join<L> {};



int main()
{
  set<char, int, double> s;
  s.values = std::tuple<char, int, double>{'c', 2, 3.4};
  set<> s2;
  std::cout << sizeof(s) << std::endl;
  std::cout << sizeof(s2) << std::endl;

  auto const s3 = s2.insert(type_<int>{23});
  std::cout << std::get<0>(s3.values) << std::endl;
  using R = insert<decltype(s2), int>;

  using J = join<decltype(s), bool, int, float>;

  set<bool, int, float> s4;
  s4.values = std::tuple<bool, int, float>{true, 5, 1.2};
  auto r = J::apply(s, s4.values);
  std::cout << std::get<3>(r.values) << std::endl;
}
