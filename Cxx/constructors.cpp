#include <iostream>
#include <algorithm>

class Terminal {};
class Add;

template<typename Op, typename E0, typename E1>
struct Binary
{
    Binary(E0&&e0, E1&&e1) { std::cout<<"ctor"<<std::endl; }
    Binary(Binary const &) { std::cout<<"cp ctor"<<std::endl; }

    Binary() { std::cout<<"null ctor"<<std::endl; }
    Binary &operator=(Binary const &) { std::cout<<"cp asgn"<<std::endl; return *this; }
};

template<typename E0, typename E1>
auto operator+(E0&&e0, E1&&e1)
{
    return Binary<Add, E0, E1>{std::forward<E0>(e0), std::forward<E1>(e1)};
}

int main()
{
    Terminal a;
    auto x = a + a; // ctor
    auto y = x;     // cp ctor
    decltype(y) z;  // null ctor
    z = y;          // cp asgn
}
