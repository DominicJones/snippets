#include <utility>
#include <memory>
#include <iostream>


struct DiffWallFunction
{
  virtual void evaluatePrimal() = 0;
};


struct WallFunction
  : virtual DiffWallFunction
{
  virtual void evaluate() = 0;
};


struct DiffBlendedWallFunction
  : virtual DiffWallFunction
{
  void evaluatePrimal() override { std::cout << __func__ << std::endl; };
};


struct BlendedWallFunction
  : WallFunction
  , virtual DiffBlendedWallFunction
{
  void evaluate() override { std::cout << __func__ << std::endl; };
};


int main()
{
  std::unique_ptr<WallFunction> object = std::make_unique<BlendedWallFunction>();
  object->evaluate();
  object->evaluatePrimal();
}
