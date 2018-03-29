#include <typeinfo>
#include <iostream>


template<class Derived_t> class Base
{
public:
  template<class U> void evaluate()
  {
    static_cast<Derived_t*>(this)->template eval<U>();

    static_cast<Derived_t*>(this)->template eval<U>(true);
    static_cast<Derived_t*>(this)->template eval<U>(false);
  }

protected:
  void echo(std::string const &msg, bool isReversed) const
  {
    if (isReversed)
      echo_reversed(msg);
    else
      echo_standard(msg);
  }

private:
  void echo_reversed(std::string const &msg) const
  {
    std::cout << std::string(msg.rbegin(), msg.rend()) << std::endl;
  }

  void echo_standard(std::string const &msg) const
  {
    std::cout << msg << std::endl;
  }
};


template<class T> class Derived
  : public Base<Derived<T> >
{
  template<class Derived_t> friend class Base;

private:
  template<class U> void eval()
  {
    eval<U>(false);
  }

  template<class U> void eval(bool isReversed)
  {
    std::string msg(typeid(*this).name());
    Base<Derived<T> >::echo(msg, isReversed);
  }
};


int main()
{
  Derived<int>().evaluate<char>();
}
