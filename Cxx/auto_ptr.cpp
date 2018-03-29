/* A memory-safe approach to getting a derived class instance
 * through a base class method; the instance is destroyed when
 * its auto_ptr goes out of scope.
 */

#include <iostream>
#include <memory>


class BaseModel
{
public:
  virtual void run() = 0;
};


class DerivedModel: public BaseModel
{
public:
  void run()
  {
    std::cout << "specific run" << std::endl;
  }
};


class BaseDriver
{
public:
  virtual std::auto_ptr<BaseModel> createModel() = 0;
  virtual void runModel() = 0;
};


class DerivedDriver: public BaseDriver
{
public:
  std::auto_ptr<BaseModel> createModel()
  {
    return std::auto_ptr<BaseModel>(new DerivedModel);
  }

  void runModel()
  {
    createModel()->run();
  }
};


int main()
{
  DerivedDriver().runModel();
}
