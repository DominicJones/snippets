#include <functional>
#include <iostream>


template<typename Functor>
void eval(Functor functor, int v)
{
   std::cout << functor(v) << std::endl;
}


auto anonymous_function = [] (int v)
{
    return v * v;
};


int named_function(int v)
{
    return v * v;
}


int main()
{
    eval([](int v){return v*v;}, 2);
    eval(anonymous_function, 4);
    eval(named_function, 6);
}
