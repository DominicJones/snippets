// Phil Nash. Lightning talk: C++ Extension Methods

#include <cassert>
#include <iostream>
 
template<typename T, typename R=void>
struct ExtMethod {
    ExtMethod& operator - () {
        return *this;
    }
    
    template<typename U>
    R operator()( U& obj ) {
        return static_cast<T*>(this)->operator()( obj );
    }
};
 
template<typename Derived>
struct Extendible
{
    template<typename T, typename ReturnT>
    ReturnT operator < ( ExtMethod<T, ReturnT>& extMethod ) {
        return extMethod( static_cast<Derived&>( *this ) );
    }
};
 
struct Widget : Extendible<Widget> {
    Widget( int size, int weight ) : size( size ), weight( weight ) {}
    
    int size;
    int weight;
};
struct print : ExtMethod<print> {
    void operator()( Widget& w ) {
        std::cout << "size: " << w.size << ", weight: " << w.weight << std::endl;
    }
};
struct density : ExtMethod<density, float> {
    float operator()( Widget& w ) {
       return (float)(w.weight / w.size);
    }
};
 
int main() {
 
    Widget w( 4, 10 );
    w<-print();
 
    float d = w<-density();
    assert( d - 10/4 < 0.01 );
}
