#include <type_traits>
#include <vector>

template<class T>
using type_t = typename T::type;

template<class T> struct identity { using type = T; };

template<class C> 
struct size_type_of : identity<typename C::size_type> { };

template<class T>
using is_vector = std::is_same<T, std::vector<typename T::value_type, typename T::allocator_type> >;
                                          
template<class C>
using size_type = typename std::conditional_t<!is_vector<C>::value, identity<int>, size_type_of<C> >::type;
            
static_assert(std::is_same<size_type<std::vector<double> >, std::size_t>::value, "");

int main() {}
