#include <iostream>
#include <memory>
#include <array>


namespace
{
  double a = 3;
  double b = 4;

  double a_drv = 0;
  double b_drv = 0;

  enum class DrvMode { PRIMAL, TANGENT, ADJOINT, };

  class Cell;

  class Continuum {};
}




template <class T>
class ExpressionVariable
{
public:
  class Engine
  {
  public:
    typedef T ConstVariable_t;
    typedef T Variable_t;
  };


  class VirtualEngine
    : public Engine
  {
  public:
    typedef int Index;
    typedef T ConstVariable_t;
    typedef T Variable_t;

    virtual ~VirtualEngine() {}
    virtual ConstVariable_t operator[](Index const &i) const = 0;
  };


  class VirtualMutableEngine
    : public VirtualEngine
  {
  public:
    using Index = int;
    using ConstVariable_t = T;

    class Variable_t
    {
    public:
      Variable_t(VirtualMutableEngine &engine, Index const &i)
        : _engine(engine)
        , _i(i)
      {}

      void operator+=(ConstVariable_t const &rhs) const
      {
        _engine.opAddAssign(_i, rhs);
      }

      void operator-=(ConstVariable_t const &rhs) const
      {
        _engine.opAddAssign(_i, ConstVariable_t(-rhs));
      }

    private:
      VirtualMutableEngine &_engine;
      Index const _i;
    };

    using VirtualEngine::operator[];

    virtual ConstVariable_t operator[](Index const &i) const override
    {
      return ConstVariable_t(0);
    }

    Variable_t operator[](Index const &i)
    {
      return Variable_t(*this, i);
    }

  protected:
    virtual void opAddAssign(Index const &i, ConstVariable_t const &rhs)
    {
      return;
    }
  };
};





class SpecificVariable
  : public ExpressionVariable<double>
{
public:
  using Engine_t = SpecificVariable::VirtualMutableEngine;

private:
  using ManagedEngine_t = std::unique_ptr<Engine_t>;

public:
  SpecificVariable(Continuum const &continuum)
    : _mode(DrvMode::PRIMAL)
  {}

  SpecificVariable(DrvMode const &mode,
                   Continuum const &continuum)
    : _mode(mode)
  {}

  template<class What>
  class SpecificEngine
  {
  public:
    using Index = Engine_t::Index;
    using ConstVariable_t = Engine_t::ConstVariable_t;
    using Variable_t = Engine_t::Variable_t;

    SpecificEngine(SpecificVariable const &outer)
      : _engine(outer.makeEngine())
      , _constEngine(const_cast<Engine_t const &>(*_engine))
    {}

    ConstVariable_t operator[](Index const &i) const
    {
      return _constEngine[i];
    }

    Variable_t operator[](Index const &i)
    {
      return (*_engine)[i];
    }

  private:
    ManagedEngine_t const _engine;
    Engine_t const &_constEngine;
  };

private:
  class PrimalEngine
    : public Engine_t
  {
  public:
    PrimalEngine(SpecificVariable const &outer) {}

    ConstVariable_t operator[](Index const &i) const final
    {
      return ConstVariable_t(a * b);
    }
  };


  class TangentEngine
    : public Engine_t
  {
  public:
    TangentEngine(SpecificVariable const &outer) {}

    ConstVariable_t operator[](Index const &i) const final
    {
      return ConstVariable_t(a_drv * b + a * b_drv);
    }
  };


  class AdjointEngine
    : public Engine_t
  {
  public:
    AdjointEngine(SpecificVariable const &outer) {}

    void opAddAssign(Index const &i, ConstVariable_t const &rhs) final
    {
      a_drv += b * rhs;
      b_drv += rhs * a;
    }
  };


  ManagedEngine_t makeEngine() const
  {
    switch (_mode) {
    case DrvMode::PRIMAL: return ManagedEngine_t(new PrimalEngine(*this));
    case DrvMode::TANGENT: return ManagedEngine_t(new TangentEngine(*this));
    case DrvMode::ADJOINT: return ManagedEngine_t(new AdjointEngine(*this));
    }
  }

  DrvMode const _mode;
};




template <class Var, class Stencil>
class Field
{
public:
  typedef typename Var::template SpecificEngine<Stencil>::Index Index;
  typedef typename Var::template SpecificEngine<Stencil>::ConstVariable_t ConstField_t;
  typedef typename Var::template SpecificEngine<Stencil>::     Variable_t      Field_t;

  explicit Field(Var const &var) : _engine(var) {}
  inline ConstField_t operator[](Index const &i) const { return _engine[i]; }
  inline      Field_t operator[](Index const &i)       { return _engine[i]; }

private:
  typename Var::template SpecificEngine<Stencil> _engine;
};


template<class Var, class Stencil>
class Field<Var const, Stencil>
{
public:
  typedef typename Var::template SpecificEngine<Stencil>::Index Index;
  typedef typename Var::template SpecificEngine<Stencil>::ConstVariable_t ConstField_t;

  explicit Field(Var const &var) : _engine(var) {}
  inline ConstField_t operator[](Index const &i) const { return _engine[i]; }

private:
  typename Var::template SpecificEngine<Stencil> const _engine;
};




template<class Var>
class FvFieldFunctionBinding
{
public:
  FvFieldFunctionBinding(Var const * var = new Var())
    : _var(var)
  {}

  template<class... Args>
  FvFieldFunctionBinding(Args const &... args)
    : _var(new Var(args...))
  {}

  ~FvFieldFunctionBinding() { delete _var; }

  Var const &var() const { return *_var; }

private:
  Var const * const _var;
};


template<class Var>
class DrvFvFieldFunctionBinding
  : public FvFieldFunctionBinding<Var>
{
public:
  template<class... Args>
  DrvFvFieldFunctionBinding(Args const &... args)
    : FvFieldFunctionBinding<Var>(Var(args...))
    , _vars{&this->var(), new Var(DrvMode::TANGENT, args...), new Var(DrvMode::ADJOINT, args...)}
  {}

  ~DrvFvFieldFunctionBinding()
  {
    delete _vars[std::size_t(DrvMode::TANGENT)];
    delete _vars[std::size_t(DrvMode::ADJOINT)];
  }

  using FvFieldFunctionBinding<Var>::var;

  Var const &var(DrvMode const &mode) const
  {
    return *_vars[std::size_t(mode)];
  }

private:
  std::array<Var const *, 3> _vars;
};




int main(int argc, char** argv)
{
  Continuum const continuum;

  {
    DrvFvFieldFunctionBinding<SpecificVariable> const binding(continuum);
  }

  {
    double c;
    SpecificVariable const var(continuum);
    Field<SpecificVariable const, Cell> field(var);
    c = field[0];
    std::cout << c << std::endl;
  }

  {
    a_drv = 1; b_drv = 0;

    double c;
    SpecificVariable const var(DrvMode::TANGENT, continuum);
    Field<SpecificVariable const, Cell> field(var);
    c = field[0];
    std::cout << c << std::endl;
  }

  {
    a_drv = 0; b_drv = 1;

    double c;
    SpecificVariable const var(DrvMode::TANGENT, continuum);
    Field<SpecificVariable const, Cell> field(var);
    c = field[0];
    std::cout << c << std::endl;
  }

  {
    a_drv = b_drv = 0;

    double c = 1;
    SpecificVariable const var(DrvMode::ADJOINT, continuum);
    Field<SpecificVariable, Cell> field(var);
    field[0] += c;
    std::cout << a_drv << " " << b_drv << std::endl;
  }
}
