// Substitution Failure Is Not An Error

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>


// enable_if is in C++11 stdlib
template <bool Cond, class T = void>
struct enable_if
{
  typedef T type;
};

template <class T>
struct enable_if<false, T>
{};


template <typename T>
class is_container
{
private:
  typedef char true_type;
  struct false_type { true_type _[2]; };
  template <typename U>
  static true_type has_iterator_checker(typename U::iterator *);
  template <typename U>
  static false_type has_iterator_checker(...);
public:
  enum { value = (sizeof(has_iterator_checker<T>(0)) == sizeof(true_type)) };
};



template <typename T>
typename enable_if<!is_container<T>::value>::type
print(T const &t)
{
  std::cout << t << std::endl;
}


template <typename T>
typename enable_if<is_container<T>::value>::type
print(T const &t)
{
  typedef typename T::value_type value_type;
  std::copy(t.begin(), t.end(), std::ostream_iterator<value_type>(std::cout, ", "));
  std::cout << std::endl;
}


int main()
{
  print(10);

  std::vector<int> b;
  b.push_back(1);
  b.push_back(2);
  b.push_back(3);
  print(b);
}
