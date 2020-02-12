// from: https://www.fluentcpp.com/2017/09/12/how-to-return-a-smart-pointer-and-use-covariance/

#include <utility>
#include <memory>
#include <iostream>


class cloneable
{
public:
  virtual ~cloneable() {}

  std::unique_ptr<cloneable> clone() const
  {
    return std::unique_ptr<cloneable>(this->clone_impl());
  }

private:
  virtual cloneable * clone_impl() const = 0;
};


template <typename Derived, typename Base>
class clone_inherit : public Base
{
public:
  std::unique_ptr<Derived> clone() const
  {
    return std::unique_ptr<Derived>(static_cast<Derived *>(this->clone_impl()));
  }

private:
  virtual clone_inherit * clone_impl() const override
  {
    return new Derived(static_cast<const Derived & >(*this));
  }
};


class concrete
  : public clone_inherit<concrete, cloneable>
{
};


int main()
{
  std::unique_ptr<concrete> c = std::make_unique<concrete>();
  std::unique_ptr<concrete> cc = c->clone();

  cloneable * p = c.get();
  std::unique_ptr<cloneable> pp = p->clone();
}
