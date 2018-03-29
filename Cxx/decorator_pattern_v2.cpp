#include <iostream>
#include <string>


class CoffeeBase {
public:
  virtual ~CoffeeBase(){};
  virtual double cost() const = 0;
  virtual std::string ingredients() const = 0;
};


class Coffee : public CoffeeBase {
public:
  double cost() const { return 1.1; }
  std::string ingredients() const { return "Coffee"; }
};


class WithMilk : public CoffeeBase {
public:
  WithMilk(CoffeeBase const &coffee)
    : _coffee(coffee) {
  }

  double cost() const {
    return _coffee.cost() + 0.5;
  }

  std::string ingredients() const {
    return _coffee.ingredients() + ", " + "Milk";
  }

private:
  CoffeeBase const &_coffee;
};


class WithWhip : public CoffeeBase {
public:
  WithWhip(CoffeeBase const &coffee)
    : _coffee(coffee) {
  }

  double cost() const {
    return _coffee.cost() + 0.7;
  }

  std::string ingredients() const {
    return _coffee.ingredients() + ", " + "Whip";
  }

private:
  CoffeeBase const &_coffee;
};


int main()
{
  Coffee s;
  std::cout << "Cost: " << s.cost() << "; Ingredients: " << s.ingredients() << std::endl;

  WithMilk m(s);
  std::cout << "Cost: " << m.cost() << "; Ingredients: " << m.ingredients() << std::endl;

  WithWhip w(s);
  std::cout << "Cost: " << w.cost() << "; Ingredients: " << w.ingredients() << std::endl;

  WithMilk m2(w);
  std::cout << "Cost: " << m2.cost() << "; Ingredients: " << m2.ingredients() << std::endl;
}
