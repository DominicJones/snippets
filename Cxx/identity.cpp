// based on boost/mpl/identity.hpp

template<class T> struct Identity { using Type = T; };
template<class T> struct Identity<Identity<T> > { using Type = T; };

class Cell;

int main()
{
  Identity<Cell> id;
  using Stencil = Identity<decltype(id)>::Type;
}
