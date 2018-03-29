#include <iostream>


template <int N>
class CN
{
public: CN(){ std::cout << "CN" << std::endl; }
};


template <typename T, int N>
class CTN
{
public: CTN(){ std::cout << "CTN" << std::endl; }
};

template <typename T>
class CTN<T, 1>
{
public: CTN(){ std::cout << "CTN: N=1" << std::endl; }
};


template <typename T>
class C0
{
public: C0(){ std::cout << "C0" << std::endl; }
};


template <typename T>
class C1
{
public: C1(){ std::cout << "C1" << std::endl; }
};

template <template <int> class T, int N>
class C1<T<N> >
{
public: C1(){ std::cout << "C1<T<N>>" << std::endl; }
};

template <template <class> class T, class U>
class C1<T<U> >
{
public: C1(){ std::cout << "C1<T<U>>" << std::endl; }
};

template <template <class, int> class T, class U, int N>
class C1<T<U, N> >
{
public: C1(){ std::cout << "C1<T<U, N>>" << std::endl; }
};


template <class T> class C_T_UI;

template <template <class, int> class T, class U, int I>
class C_T_UI<T<U, I> >
{
public: C_T_UI(){ std::cout << "C<T<U, I>>" << std::endl; }
};

template <template <class, int> class T, class U>
class C_T_UI<T<U, 3> >
{
public: C_T_UI(){ std::cout << "C<T<U, 3>>" << std::endl; }
};


int main() {
  CTN<int,1> ctn1;
  CTN<int,2> ctn;
  C1<std::string> c1;
  C1<CN<3> > c1b;
  C1<C0<int> > c1c;
  C1<CTN<int, 3> > c1d;
  C_T_UI< CTN<int, 2> > c_t_ui;
  C_T_UI< CTN<int, 3> > c_t_ui3;
}
