#include <cassert>
#include <cmath>
#include <type_traits>
#include <memory>
#include <array>
#include <map>
#include <string>
#include <limits>
#include <iostream>



struct Access_t {};
struct Input_t: Access_t {};
struct Output_t: Access_t {};

struct Mode_t {};
template<class UniqueKey> struct DupPrimal_t: Mode_t {};
struct Primal_t: Mode_t {};
struct Tangent_t: Mode_t {};
struct Adjoint_t: Mode_t {};

template<class Mode, class Access> struct IsInputType
{
  static bool const value = (std::is_same<Access, Input_t>::value  && !std::is_same<Mode, Adjoint_t>::value ||
                             std::is_same<Access, Output_t>::value &&  std::is_same<Mode, Adjoint_t>::value);
};

template<class Mode, class Access> struct IsOutputType
{
  static bool const value = (std::is_same<Access, Output_t>::value && !std::is_same<Mode, Adjoint_t>::value ||
                             std::is_same<Access, Input_t>::value  &&  std::is_same<Mode, Adjoint_t>::value);
};



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



class Function
{
public:
  void evaluate() const;

private:
  void evalVariance() const;
  void evalStdDeviation() const;
};

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



class FunctionDerivative
{
public:
  void evaluate(Primal_t) const;
  void evaluate(Tangent_t) const;
  void evaluate(Adjoint_t) const;

  template<class Mode> struct Evaluate
  {
    template<class Access> void initialize() const;
    template<class Access> void free() const;
    void stashOutput() const;
    void restoreOutput() const;
    void execute() const;
  private:
    typedef DupPrimal_t<Evaluate> DupPrimal;
  };

private:
  void evalVariance(Primal_t) const;
  void evalVariance(Tangent_t) const;
  void evalVariance(Adjoint_t) const;

  template<class Mode> struct EvalVariance
  {
    template<class Access> void initialize() const;
    template<class Access> void free() const;
    void stashOutput() const;
    void restoreOutput() const;
    void execute() const;
  private:
    typedef DupPrimal_t<EvalVariance> DupPrimal;
  };

  void evalStdDeviation(Primal_t) const;
  void evalStdDeviation(Tangent_t) const;
  void evalStdDeviation(Adjoint_t) const;

  template<class Mode> struct EvalStdDeviation
  {
    template<class Access> void initialize() const;
    template<class Access> void free() const;
    void stashOutput() const;
    void restoreOutput() const;
    void execute() const;
  private:
    typedef DupPrimal_t<EvalStdDeviation> DupPrimal;
  };
};


void FunctionDerivative::evaluate(Primal_t primal) const
{
  evalVariance(primal);
  evalStdDeviation(primal);
}

void FunctionDerivative::evaluate(Tangent_t tangent) const
{
  evalVariance(tangent);
  evalStdDeviation(tangent);
}

void FunctionDerivative::evaluate(Adjoint_t adjoint) const
{
  evalVariance(Primal_t());
  evalStdDeviation(adjoint);
  evalVariance(adjoint);
}


void FunctionDerivative::evalVariance(Primal_t) const
{
  EvalVariance<Primal_t>().execute();
}

void FunctionDerivative::evalVariance(Tangent_t) const
{
  EvalVariance<Tangent_t>().execute();
}

void FunctionDerivative::evalVariance(Adjoint_t) const
{
  EvalVariance<Adjoint_t>().execute();
}


void FunctionDerivative::evalStdDeviation(Primal_t) const
{
  EvalStdDeviation<Primal_t>().execute();
}

void FunctionDerivative::evalStdDeviation(Tangent_t) const
{
  EvalStdDeviation<Tangent_t>().execute();
}

void FunctionDerivative::evalStdDeviation(Adjoint_t) const
{
  EvalStdDeviation<Adjoint_t>().execute();
}


template<class Mode> template<class Access> void FunctionDerivative::Evaluate<Mode>::initialize() const
{
  EvalVariance<Mode>().initialize<Access>();
}

template<class Mode> template<class Access> void FunctionDerivative::Evaluate<Mode>::free() const
{
  EvalVariance<Mode>().free<Access>();
}

template<class Mode> void FunctionDerivative::Evaluate<Mode>::stashOutput() const
{
  EvalVariance<Mode>().stashOutput();
}

template<class Mode> void FunctionDerivative::Evaluate<Mode>::restoreOutput() const
{
  EvalVariance<Mode>().restoreOutput();
}

template<class Mode> void FunctionDerivative::Evaluate<Mode>::execute() const
{
  evaluate(Mode());
}


template<class Mode> template<class Access> void FunctionDerivative::EvalVariance<Mode>::initialize() const
{
  if (IsInputType<Mode, Access>::value && !DrvValues<Mode>().hasStorage())
    {
      DrvValues<Mode>().addStorage();
      DrvValues<Mode> values_drv;
      values_drv[0] = values_drv[1] = 0;
    }

  if (IsOutputType<Mode, Access>::value && !DrvStdDeviation<Mode>().hasStorage())
    {
      DrvStdDeviation<Mode>().addStorage();
      DrvStdDeviation<Mode> stdDeviation_drv;
      stdDeviation_drv[0] = 0;
    }
}

template<class Mode> template<class Access> void FunctionDerivative::EvalVariance<Mode>::free() const
{
  if (IsInputType<Mode, Access>::value)
    {
      DrvValues<Mode>().removeStorage();
    }

  if (IsOutputType<Mode, Access>::value)
    {
      DrvStdDeviation<Mode>().removeStorage();
    }
}

template<class Mode> void FunctionDerivative::EvalVariance<Mode>::stashOutput() const
{
  StdDeviation stdDeviation;
  DrvStdDeviation<DupPrimal> stdDeviation_dup;
  stdDeviation_dup.addStorage();
  stdDeviation_dup[0] = stdDeviation[0];
}

template<class Mode> void FunctionDerivative::EvalVariance<Mode>::restoreOutput() const
{
  StdDeviation stdDeviation;
  DrvStdDeviation<DupPrimal> stdDeviation_dup;
  stdDeviation[0] = stdDeviation_dup[0];
  stdDeviation_dup.removeStorage();
}

template<class Mode> void FunctionDerivative::EvalVariance<Mode>::execute() const
{
  Values values;
  DrvValues<Mode> values_drv;

  StdDeviation stdDeviation;
  DrvStdDeviation<Mode> stdDeviation_drv;

  stdDeviation.addStorage();
  stdDeviation[0] = 0;
  stdDeviation[0] += pow(values[0], 2);
  stdDeviation[0] += pow(values[1], 2);

  if (std::is_same<Mode, Tangent_t>::value)
    {
      stdDeviation_drv.addStorage();
      stdDeviation_drv[0] = 0;
      stdDeviation_drv[0] += (2*values[0])*values_drv[0];
      stdDeviation_drv[0] += (2*values[1])*values_drv[1];
    }
  if (std::is_same<Mode, Adjoint_t>::value)
    {
      values_drv[0] += (2*values[0])*stdDeviation_drv[0];
      values_drv[1] += (2*values[1])*stdDeviation_drv[0];
      stdDeviation_drv[0] = 0;
      stdDeviation_drv.removeStorage();
    }
}


template<class Mode> template<class Access> void FunctionDerivative::EvalStdDeviation<Mode>::initialize() const
{
  if (std::is_same<Mode, Adjoint_t>::value)
    EvalVariance<Mode>().initialize<Input_t>();
  else
    EvalVariance<Mode>().initialize<Output_t>();
}

template<class Mode> template<class Access> void FunctionDerivative::EvalStdDeviation<Mode>::free() const
{
  if (std::is_same<Mode, Adjoint_t>::value)
    EvalVariance<Mode>().free<Input_t>();
  else
    EvalVariance<Mode>().free<Output_t>();
}

template<class Mode> void FunctionDerivative::EvalStdDeviation<Mode>::stashOutput() const
{
  EvalVariance<Mode>().stashOutput();
}

template<class Mode> void FunctionDerivative::EvalStdDeviation<Mode>::restoreOutput() const
{
  EvalVariance<Mode>().stashOutput();
}

template<class Mode> void FunctionDerivative::EvalStdDeviation<Mode>::execute() const
{
  StdDeviation stdDeviation;

  DrvStdDeviation<Mode> stdDeviation_drv;

  DrvStdDeviation<DupPrimal> stdDeviation_dup;
  stdDeviation_dup.addStorage();
  stdDeviation_dup[0] = sqrt(stdDeviation[0]);

  if (std::is_same<Mode, Tangent_t>::value)
    {
      stdDeviation_drv[0] += stdDeviation_drv[0] / (2*sqrt(stdDeviation[0]));
    }
  if (std::is_same<Mode, Adjoint_t>::value)
    {
      stdDeviation_drv[0] += stdDeviation_drv[0] / (2*sqrt(stdDeviation[0]));
    }

  stdDeviation[0] = stdDeviation_dup[0];
  stdDeviation_dup.removeStorage();
}



int main()
{
  auto isEqual = [](double a, double b)
    {
      double delta = std::numeric_limits<double>::round_error();
      double c = a - b;
      return (c < delta) && (-c < delta);
    };


  Values values;
  DrvValues<Tangent_t> values_tng;
  DrvValues<Adjoint_t> values_adj;

  StdDeviation stdDeviation;
  DrvStdDeviation<Tangent_t> stdDeviation_tng;
  DrvStdDeviation<Adjoint_t> stdDeviation_adj;

  Function function;
  FunctionDerivative functionDerivative;


  auto primalTest = [&]()
    {
      // reference primal test
      FunctionDerivative::Evaluate<Primal_t>().free<Input_t>();
      FunctionDerivative::Evaluate<Primal_t>().initialize<Input_t>();
      values[0] = 3; values[1] = 4;

      function.evaluate();
      assert(isEqual(stdDeviation[0], 5));


      // derivative-based primal test
      functionDerivative.evaluate(Primal_t());
      assert(isEqual(stdDeviation[0], 5));
    };

  primalTest();
  primalTest();


  auto tangentTest = [&]()
    {
      // tangent only test
      stdDeviation[0] = -1;
      FunctionDerivative::Evaluate<Primal_t>().stashOutput();

      FunctionDerivative::Evaluate<Tangent_t>().free<Input_t>();
      FunctionDerivative::Evaluate<Tangent_t>().initialize<Input_t>();
      values_tng[0] = 1;
      functionDerivative.evaluate(Tangent_t());
      assert(isEqual(stdDeviation_tng[0], 6.6));

      FunctionDerivative::Evaluate<Tangent_t>().free<Input_t>();
      FunctionDerivative::Evaluate<Tangent_t>().initialize<Input_t>();
      values_tng[1] = 1;
      functionDerivative.evaluate(Tangent_t());
      assert(isEqual(stdDeviation_tng[0], 8.8));

      FunctionDerivative::Evaluate<Primal_t>().restoreOutput();
      assert(isEqual(stdDeviation[0], -1));
    };

  tangentTest();
  tangentTest();


  auto adjointTest = [&]()
    {
      // adjoint only test
      stdDeviation[0] = -1;
      FunctionDerivative::Evaluate<Primal_t>().stashOutput();

      FunctionDerivative::Evaluate<Adjoint_t>().free<Input_t>();
      FunctionDerivative::Evaluate<Adjoint_t>().initialize<Input_t>();
      stdDeviation_adj[0] = 1;

      FunctionDerivative::Evaluate<Adjoint_t>().free<Output_t>();
      FunctionDerivative::Evaluate<Adjoint_t>().initialize<Output_t>();
      functionDerivative.evaluate(Adjoint_t());
      assert(isEqual(values_adj[0], 6.6));
      assert(isEqual(values_adj[1], 8.8));

      FunctionDerivative::Evaluate<Primal_t>().restoreOutput();
      assert(isEqual(stdDeviation[0], -1));
    };

  adjointTest();
  adjointTest();
}
