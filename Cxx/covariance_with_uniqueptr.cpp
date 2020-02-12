// from: https://www.fluentcpp.com/2017/09/12/how-to-return-a-smart-pointer-and-use-covariance/

#include <utility>
#include <memory>
#include <iostream>


template <typename T> class abstract_method {};


template <typename T>
class virtual_inherit_from : virtual public T { using T::T; };


template <typename Derived, typename ... Bases>
class clone_inherit : public Bases...
{
 public:
  virtual ~clone_inherit() = default;

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


template <typename Derived, typename ... Bases>
class clone_inherit<abstract_method<Derived>, Bases...> : public Bases...
{
 public:
  virtual ~clone_inherit() = default;

  std::unique_ptr<Derived> clone() const
  {
    return std::unique_ptr<Derived>(static_cast<Derived *>(this->clone_impl()));
  }

 private:
  virtual clone_inherit * clone_impl() const = 0;
};


template <typename Derived>
class clone_inherit<Derived>
{
public:
  virtual ~clone_inherit() = default;

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


template <typename Derived>
class clone_inherit<abstract_method<Derived>>
{
 public:
  virtual ~clone_inherit() = default;

  std::unique_ptr<Derived> clone() const
  {
    return std::unique_ptr<Derived>(static_cast<Derived *>(this->clone_impl()));
  }

 private:
  virtual clone_inherit * clone_impl() const = 0;
};


// user code


class cloneable
  : public clone_inherit<abstract_method<cloneable>>
{};

class base1
  : public clone_inherit<abstract_method<base1>, virtual_inherit_from<cloneable>>
{};

class base2
  : public clone_inherit<abstract_method<base2>, virtual_inherit_from<cloneable>>
{};

class concrete
  : public clone_inherit<concrete, base1, base2>
{};

int main()
{
  std::unique_ptr<concrete> c = std::make_unique<concrete>();
  std::unique_ptr<concrete> cc = c->clone();

  base1 * f = c.get();
  std::unique_ptr<base1> ff = c->clone();

  base2 * b = c.get();
  std::unique_ptr<base2> bb = c->clone();

  cloneable * p = c.get();
  std::unique_ptr<cloneable> pp = p->clone();
}
