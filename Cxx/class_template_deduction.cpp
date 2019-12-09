/*
template <class T> struct A
{
  using value_type = T;
  A(value_type){}; // #1
  A(const A&){}; // #2
  A(T, T, int){}; // #3
  template<class U> A(int, T, U){}; // #4
}; // A(A); #5, the copy deduction candidate

template <class T> A(T) -> A<T>;  // #6, less specialized than #5
template <class T> A(A<T>) -> A<A<T>>;  // #7, as specialized as #5

int main()
{
  A x(1, 2, 3); // uses #3, generated from a non-template constructor
  A a(42); // uses #6 to deduce A<int> and #1 to initialize
  A b = a;  // uses #5 to deduce A<int> and #2 to initialize
  A b2 = a;  // uses #7 to deduce A<A<int>> and #1 to initialize
}
*/


template<class T, class E>
struct Drv
{
  Drv(E const &){}
};

int main()
{
  Drv<double> x(1.0); // error: partial specialization not supported
  // https://stackoverflow.com/questions/57563594/partial-class-template-argument-deduction-in-c17
}
