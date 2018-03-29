/* A use of the "Curiously Recurring Template Pattern"
 *
 * This example demonstrates a workaround to the problem
 * of not being able to have template pure-virtual methods
 * in a base class.
 *
 * In this example 'eval' algorithm is not being repeated
 * in each derived class. Duplication of 'eval' would be
 * difficult to avoid without this pattern.
 */


#include <iostream>
using namespace std;


template<class _B_>
struct A
{
  void eval(bool _3d = true)
  {
    if (_3d)
      static_cast<_B_*>(this)->template calc<3>();
    else
      static_cast<_B_*>(this)->template calc<2>();
  }
};


struct B_v1 : public A<B_v1>
{
  template<int> void calc(){ cout << "version 1" << endl; }
};


struct B_v2 : public A<B_v2>
{
  template<int> void calc(){ cout << "version 2" << endl; }
};


int main()
{
  B_v1().eval(); // prints "version 1"
  B_v2().eval(); // prints "version 2"
}
