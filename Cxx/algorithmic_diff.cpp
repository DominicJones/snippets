#include <cassert>
#include <cmath>
#include <type_traits>
#include <memory>
#include <array>
#include <map>
#include <string>
#include <limits>



template<class num_type, int size> class Storage
{
public:
  bool hasStorage() const;
  void addStorage();
  void removeStorage();
  num_type operator[](size_t i) const { return _data[key()][i]; }
  num_type &operator[](size_t i) { return _data[key()][i]; }
protected:
  virtual size_t key() const = 0;
private:
  static std::map<size_t, std::array<num_type, size>> _data;
};

template<class num_type, int size>
std::map<size_t, std::array<num_type, size>> Storage<num_type, size>::_data;

template<class num_type, int size> bool Storage<num_type, size>::hasStorage() const
{
  auto it = _data.find(key());
  return it != _data.end();
}

template<class num_type, int size> void Storage<num_type, size>::addStorage()
{
  if (!hasStorage())
    {
      double const sNan = std::numeric_limits<num_type>::signaling_NaN();
      _data[key()] = std::array<num_type, size>{sNan};
    }
}

template<class num_type, int size> void Storage<num_type, size>::removeStorage()
{
  _data.erase(key());
}



class Values: public Storage<double, 2>
{
public:
  size_t key() const { return typeid(decltype(*this)).hash_code(); }
};

class StdDeviation: public Storage<double, 1>
{
public:
  size_t key() const { return typeid(decltype(*this)).hash_code(); }
};



struct Access_t {};
struct Input_t: Access_t {};
struct Output_t: Access_t {};

struct Mode_t {};
template<class Snapshot> struct DupPrimal_t: Mode_t {};
struct Primal_t: Mode_t { static const bool UpdatePrimal = true; };
struct Tangent_t: Mode_t { static const bool UpdatePrimal = true; };
struct Adjoint_t: Mode_t { static const bool UpdatePrimal = false; };



template<class Mode> class DrvValues: public Values
{
public:
  size_t key() const {
    if (std::is_same<Mode, Primal_t>::value)
      return typeid(Values).hash_code();
    else
      return typeid(decltype(*this)).hash_code();
  }
};

template<class Mode> class DrvStdDeviation: public StdDeviation
{
public:
  size_t key() const {
    if (std::is_same<Mode, Primal_t>::value)
      return typeid(StdDeviation).hash_code();
    else
      return typeid(decltype(*this)).hash_code();
  }
};



template<class Mode, bool PrimalOnly> struct UpdatePrimal
{
  static int const value = Mode::UpdatePrimal || PrimalOnly;
};

template<class Mode, bool PrimalOnly> struct UpdateTangent
{
  static int const value = std::is_same<Mode, Tangent_t>::value && !PrimalOnly;
};

template<class Mode, bool PrimalOnly> struct UpdateAdjoint
{
  static int const value = std::is_same<Mode, Adjoint_t>::value && !PrimalOnly;
};



class FunctionDerivative;

class DifferentiableFunction
{
public:
  virtual ~DifferentiableFunction() {}
  virtual FunctionDerivative const &derivative() const = 0;
};



class Function : public DifferentiableFunction
{
  friend class FunctionDerivative;

public:
  Function();
  void evaluate() const;
  FunctionDerivative const &derivative() const;

private:
  void evalVariance() const;
  void evalStdDeviation() const;

  class EvalVarianceHelper {
  public:
    template<class Mode, class Access> void initialize() const;
    void stashOutput();
    void restoreOutput();
  private:
    typedef DupPrimal_t<EvalVarianceHelper> DupPrimal;
  };

  class EvalStdDeviationHelper {
  public:
    template<class Mode, class Access> void initialize() const;
    void stashOutput();
    void restoreOutput();
  private:
    typedef DupPrimal_t<EvalStdDeviationHelper> DupPrimal;
  };

  std::unique_ptr<FunctionDerivative> _derivative;
};



class FunctionDerivative
{
public:
  FunctionDerivative(Function &outer);
  void evaluate(Primal_t) const;
  void evaluate(Tangent_t) const;
  void evaluate(Adjoint_t) const;

private:
  template<class Mode> struct Evaluate {
    Evaluate();
    template<bool PrimalOnly> void evalVariance() const;
    template<bool PrimalOnly> void evalStdDeviation() const;
  };

  Function &_outer;
};



template<class Mode, class Access>
void Function::EvalVarianceHelper::initialize() const
{
  bool forceZero = std::is_same<Mode, Primal_t>::value;

  if (std::is_same<Access, Input_t>::value && !std::is_same<Mode, Adjoint_t>::value ||
      std::is_same<Access, Output_t>::value && std::is_same<Mode, Adjoint_t>::value)
    {
      if (forceZero || !DrvValues<Mode>().hasStorage()) {
        DrvValues<Mode>().addStorage();
        DrvValues<Mode> values_drv;
        values_drv[0] = values_drv[1] = 0;
      }
    }

  if (std::is_same<Access, Output_t>::value && !std::is_same<Mode, Adjoint_t>::value ||
      std::is_same<Access, Input_t>::value && std::is_same<Mode, Adjoint_t>::value)
    {
      if (forceZero || !DrvStdDeviation<Mode>().hasStorage()) {
        DrvStdDeviation<Mode>().addStorage();
        DrvStdDeviation<Mode> stdDeviation_drv;
        stdDeviation_drv[0] = 0;
      }
    }
}

void Function::EvalVarianceHelper::stashOutput()
{
  StdDeviation stdDeviation;
  DrvStdDeviation<DupPrimal> stdDeviation_dup;
  stdDeviation_dup.addStorage();
  stdDeviation_dup[0] = stdDeviation[0];
}

void Function::EvalVarianceHelper::restoreOutput()
{
  StdDeviation stdDeviation;
  DrvStdDeviation<DupPrimal> stdDeviation_dup;
  stdDeviation[0] = stdDeviation_dup[0];
  stdDeviation_dup.removeStorage();
}


void Function::EvalStdDeviationHelper::stashOutput()
{
  StdDeviation stdDeviation;
  DrvStdDeviation<DupPrimal> stdDeviation_dup;
  stdDeviation_dup.addStorage();
  stdDeviation_dup[0] = stdDeviation[0];
}

void Function::EvalStdDeviationHelper::restoreOutput()
{
  StdDeviation stdDeviation;
  DrvStdDeviation<DupPrimal> stdDeviation_dup;
  stdDeviation[0] = stdDeviation_dup[0];
  stdDeviation_dup.removeStorage();
}



Function::Function()
{
  _derivative =
    std::unique_ptr<FunctionDerivative>(new FunctionDerivative(*this));
}



FunctionDerivative::FunctionDerivative(Function &outer)
: _outer(outer)
{}



FunctionDerivative const &Function::derivative() const
{
  return *_derivative;
}

void Function::evaluate() const
{
  evalVariance();
  evalStdDeviation();
}

void Function::evalVariance() const
{
  Values values;
  StdDeviation stdDeviation;
  stdDeviation.addStorage();
  stdDeviation[0] = 0;
  stdDeviation[0] += pow(values[0], 2);
  stdDeviation[0] += pow(values[1], 2);
}

void Function::evalStdDeviation() const
{
  StdDeviation stdDeviation;
  stdDeviation[0] = sqrt(stdDeviation[0]);
}



template<class Mode>
FunctionDerivative::Evaluate<Mode>::Evaluate()
{
  typedef typename std::common_type<Mode, Mode_t>::type type;
  static_assert(std::is_constructible<type>::value,
                "invalid derivative type");
}

template<class Mode>
template<bool PrimalOnly>
void FunctionDerivative::Evaluate<Mode>::evalVariance() const
{
  Values values;
  DrvValues<Mode> values_drv;

  StdDeviation stdDeviation;
  DrvStdDeviation<Mode> stdDeviation_drv;

  if (UpdateTangent<Mode, PrimalOnly>::value)
    {
      stdDeviation_drv.addStorage();
      stdDeviation_drv[0] = 0;
      stdDeviation_drv[0] += (2*values[0])*values_drv[0];
      stdDeviation_drv[0] += (2*values[1])*values_drv[1];
    }
  if (UpdateAdjoint<Mode, PrimalOnly>::value)
    {
      values_drv[0] += (2*values[0])*stdDeviation_drv[0];
      values_drv[1] += (2*values[1])*stdDeviation_drv[0];
      stdDeviation_drv[0] = 0;
      stdDeviation_drv.removeStorage();
    }

  if (UpdatePrimal<Mode, PrimalOnly>::value)
    {
      stdDeviation.addStorage();
      stdDeviation[0] = 0;
      stdDeviation[0] += pow(values[0], 2);
      stdDeviation[0] += pow(values[1], 2);
    }
}

template<class Mode>
template<bool PrimalOnly>
void FunctionDerivative::Evaluate<Mode>::evalStdDeviation() const
{
  StdDeviation stdDeviation;
  DrvStdDeviation<Mode> stdDeviation_drv;

  if (UpdateTangent<Mode, PrimalOnly>::value)
    {
      stdDeviation_drv[0] += stdDeviation_drv[0] / (2*sqrt(stdDeviation[0]));
    }
  if (UpdateAdjoint<Mode, PrimalOnly>::value)
    {
      stdDeviation_drv[0] += stdDeviation_drv[0] / (2*sqrt(stdDeviation[0]));
    }

  if (UpdatePrimal<Mode, PrimalOnly>::value)
    {
      stdDeviation[0] = sqrt(stdDeviation[0]);
    }
}

void FunctionDerivative::evaluate(Primal_t) const
{
  Evaluate<Primal_t> evaluate;
  evaluate.evalVariance<true>();
  evaluate.evalStdDeviation<true>();
}

void FunctionDerivative::evaluate(Tangent_t) const
{
  Evaluate<Tangent_t> evaluate;

  Function::EvalVarianceHelper().initialize<Tangent_t, Output_t>();
  Function::EvalVarianceHelper().stashOutput();
  evaluate.evalVariance<false>();

  Function::EvalStdDeviationHelper().stashOutput();
  evaluate.evalStdDeviation<false>();

  Function::EvalStdDeviationHelper().restoreOutput();
  Function::EvalVarianceHelper().restoreOutput();
}

void FunctionDerivative::evaluate(Adjoint_t) const
{
  Evaluate<Adjoint_t> evaluate;

  Function::EvalVarianceHelper().initialize<Adjoint_t, Output_t>();
  Function::EvalVarianceHelper().stashOutput();
  evaluate.evalVariance<true>();

  evaluate.evalStdDeviation<false>();
  evaluate.evalVariance<false>();
  Function::EvalVarianceHelper().restoreOutput();
}



bool isEqual(double a, double b, double const &epsilon = 1.0e-5)
{
  double c = a - b;
  return (c < epsilon) && (-c < epsilon);
}



int main()
{
  Values values;
  DrvValues<Tangent_t> values_tng;
  DrvValues<Adjoint_t> values_adj;

  StdDeviation stdDeviation;
  DrvStdDeviation<Tangent_t> stdDeviation_tng;
  DrvStdDeviation<Adjoint_t> stdDeviation_adj;

  Function * function = new Function();
  assert(dynamic_cast<DifferentiableFunction *>(function));

  values.addStorage();
  values[0] = 3; values[1] = 4;

  {
    // reference primal test
    function->evaluate();
    assert(isEqual(stdDeviation[0], 5));


    // derivative-based primal test
    function->derivative().evaluate(Primal_t());
    assert(isEqual(stdDeviation[0], 5));
  }

  {
    // tangent only test
    stdDeviation[0] = -1;

    values_tng.addStorage();
    values_tng[0] = 1; values_tng[1] = 0;
    function->derivative().evaluate(Tangent_t());
    assert(isEqual(stdDeviation_tng[0], 6.6));

    values_tng[0] = 0; values_tng[1] = 1;
    function->derivative().evaluate(Tangent_t());
    assert(isEqual(stdDeviation_tng[0], 8.8));

    assert(isEqual(stdDeviation[0], -1));
  }

  {
    // adjoint only test
    stdDeviation[0] = -1;

    stdDeviation_adj.addStorage();
    stdDeviation_adj[0] = 1;

    function->derivative().evaluate(Adjoint_t());
    assert(isEqual(values_adj[0], 6.6));
    assert(isEqual(values_adj[1], 8.8));

    assert(isEqual(stdDeviation[0], -1));
  }

  // A mechanism is required to force the zeroing of all
  // adjoint storage so as to recompute the adjoint solution.
}
