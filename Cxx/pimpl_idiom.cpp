// Extend a class to implement its tangent and
// adjoint derivatives using the Pimpl idiom,
// interfaces and the RAII idiom.

#include <memory>


class Mode
{
public:
  enum Option { Primal, Tangent, Adjoint, };
};


class DerivativeBase
{
public:
  virtual ~DerivativeBase() {}
  virtual void initialize(Mode::Option modeOption) const = 0;
  virtual void evaluate(Mode::Option modeOption) const = 0;
};


class Differentiable
{
public:
  virtual ~Differentiable() {}
  virtual DerivativeBase const & derivative() const = 0;
};


class Function : public Differentiable
{
public:
  Function();
  void evaluate() const;
  DerivativeBase const & derivative() const;
private:
  class Derivative;
  std::auto_ptr<Derivative> _derivative;
};


class Function::Derivative : public DerivativeBase
{
public:
  Derivative(Function &outer): _outer(outer) {}
  void initialize(Mode::Option modeOption) const;
  void evaluate(Mode::Option modeOption) const;
private:
  Function &_outer;
};


Function::Function()
{
  _derivative = std::auto_ptr<Derivative>(new Derivative(*this));
}

DerivativeBase const & Function::derivative() const
{
  return *_derivative;
}

void Function::evaluate() const
{
}

void Function::Derivative::initialize(Mode::Option modeOption) const
{
  if (modeOption == Mode::Adjoint) {
    // FunctionDerivative<AdjointMode>().initialize();
  }
}

void Function::Derivative::evaluate(Mode::Option modeOption) const
{
  if (modeOption == Mode::Primal) {
    _outer.evaluate();
  }
  else if (modeOption == Mode::Tangent) {
    // FunctionDerivative<TangentMode>().evaluate();
  }
  else if (modeOption == Mode::Adjoint) {
    // FunctionDerivative<AdjointMode>().evaluate();
  }
}


int main()
{
  Function * function = new Function();

  if (dynamic_cast<Differentiable *>(function))
    {
      function->derivative().initialize(Mode::Adjoint);
      function->derivative().evaluate(Mode::Adjoint);
    }
}
