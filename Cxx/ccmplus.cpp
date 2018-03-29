/*
 * examples:
 *   01. move, dead reference prevention, [delete]
 *   02. object slicing, unique_ptr, [override], [final]
 *   03. perfect forwarding, variadic arguments
 *   04. integer sequence, variadic arguments, tuple
 *   05. hash combine
 *   06. class traits
 *   07. template class traits
 *   08. function traits
 *   09. run-time to compile-time select
 *   10. class template parameter naming
 *   11. class layout (not for compiling)
 *   12. static casting
 *   13. object slicing
 *
 * notes:
 *   compile with -std c++11, except for integer_sequence
 *   example which requires c++14
 */
#define EXAMPLE_01


#include <tuple>
#include <array>
#include <vector>
#include <utility>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <iostream>








#ifdef EXAMPLE_01
class Term
{
public:
  // always use the initialiser list of the ctor
  Term(std::vector<int> const &data): _data(data) {}

  // prevent a temporary being passed to the ctor
  Term(std::vector<int> const &&data) = delete;

  // return by value (temporary is moved, not duplicated)
  std::vector<int> map() const
  {
    std::vector<int> result(_data.size(), 0);

    for (int it = 0; it != _data.size(); ++it)
      result[it] = _data[it] * _data[it];

    return result;
  }

private:
  // const qualified prevents implicit copy ctor generation
  // never reference a temporary from the ctor -
  //   temporary goes out of scope after the ctor is executed
  std::vector<int> const &_data;
};


int main()
{
  std::vector<int> const data(3, 100);
  auto const result(Term(data).map());
  for (auto const &e: result)
    std::cout << e << std::endl;
}
#endif








/*
 * move semantics:
 *   obtain heap-based objects without deep copies
 *   distinguish named objects from unnamed objects
 *   facilitate perfect forwarding
 *
 * use:
 *   Type && object
 *   std::vector<Type>().emplace_back(args)
 */
#ifdef EXAMPLE_02
struct Operator
{
  // normally would be pure-virtual
  virtual double operator()(double const &value) const
  {
    return value;
  }
};


struct Pow2 : Operator
{
  // qualified with 'final' (but could be 'override')
  double operator()(double const &value) const final
  {
    return value * value;
  }
};


class Term
{
public:
  // take ownership of 'op'
  Term(std::vector<int> const &data,
       std::unique_ptr<Operator> op)
    : _data(data), _op(std::move(op)) {}

  Term(std::vector<int> const &&data,
       std::unique_ptr<Operator> op) = delete;

  std::vector<int> map() const
  {
    std::vector<int> result(_data.size(), 0);

    for (int it = 0; it != _data.size(); ++it)
      result[it] = (*_op)(_data[it]);

    return result;
  }

private:
  std::vector<int> const &_data;
  
  // unique_ptr manages 'op' so as to not
  // 'slice' it when calling its virtual method
  std::unique_ptr<Operator> const _op;
};


int main()
{
  std::vector<int> const data(3, 100);
  auto const result(Term(data, std::make_unique<Pow2>()).map());
  for (auto const &e: result)
    std::cout << e << std::endl;
}
#endif








/*
 * variadic templates:
 *   facilitate generic perfect forwarding
 *   argument capturing
 *
 * use:
 *   std::forward<Args>(args)...
 *   std::tuple<Args...> args
 */
#ifdef EXAMPLE_03
struct Base
{
  Base(char const &arg0,
       int const &arg1)
  {}

  Base(char const &arg0,
       int const &arg1,
       double const &arg2)
  {}

  virtual ~Base() = default;
};


// before: Derived is-a Base
// after: Derived is-a Intermediate is-a Base
template<typename Result>
struct Intermediate : Base
{
  // standard layout of perfect forwarding
  template<typename... Args>
  Intermediate(Args &&... args)
    : Base(std::forward<Args>(args)...)
  {}
};


template<typename Input, typename Result>
struct Derived : Intermediate<Result>
{
  Derived(char const &arg0,
          int const &arg1)
    : Intermediate<Result>(arg0, arg1)
  {}

  Derived(char const &arg0,
          int const &arg1,
          double const &arg2)
    : Intermediate<Result>(arg0, arg1, arg2)
  {}
};


int main()
{
  Derived<std::array<float, 3>, float>('a', 1, 2.0);
}
#endif








/*
 * integer sequence:
 *   access variadic templates via index
 *
 * use:
 *   [sudo] foreach(Arg in Args): eval<Arg>()
 */
#ifdef EXAMPLE_04
class Pressure {};

class Cell {};
class BFace {};


// could be improved to accept at least one 'Stencil'...
template<typename Variable, typename... Stencil>
class Bind
{
public:
  void evaluate() const
  {
    // should normally use 'std::decay' here...
    using Stencils = std::tuple<Stencil...>;

    constexpr auto size(std::tuple_size<Stencils>::value);
    
    // recursively sequence through all the 'Stencils'
    eval(std::make_index_sequence<size>{});
  }

private:
  // no-terminal recursion 'eval'
  template<std::size_t... index>
  typename std::enable_if<bool(std::index_sequence<index...>::size() != 0)>::type
  eval(std::index_sequence<index...>) const
  {
    constexpr auto size(std::index_sequence<index...>::size());
    
    // sequence the 'Stencils' in the order they were listed
    // to reverse the order, make this the last line of this method
    eval(std::make_index_sequence<size - 1>{});

    using Stencils = std::tuple<Stencil...>;
    using Stencil_i = typename std::tuple_element<size - 1, Stencils>::type;

    // do something with Stencil_i
  }

  // terminal recursion 'eval'
  template<std::size_t... index>
  typename std::enable_if<bool(std::index_sequence<index...>::size() == 0)>::type
  eval(std::index_sequence<index...>) const
  {
    // no-op
  }
};


int main()
{
  Bind<Pressure>().evaluate();
  Bind<Pressure, Cell>().evaluate();
  Bind<Pressure, Cell, BFace>().evaluate();
}
#endif









#ifdef EXAMPLE_05

template<class T>
inline void hash_combine(std::size_t &seed, T const &value)
{
  std::hash<T> hasher;
  seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


int main()
{
  int a = 1;
  int b = 2;
  size_t hash = 0;
  hash_combine(hash, a);
  hash_combine(hash, b);
  std::cout << hash << std::endl;
}

#endif








#ifdef EXAMPLE_06

template<typename, typename = void>
struct has_data : std::false_type {};

template<typename T>
struct has_data<T, std::void_t<decltype(T::data)> > : std::true_type {};


template<typename, typename = void>
struct has_method : std::false_type {};

template<typename T>
struct has_method<T, std::void_t<decltype(&T::method)> > : std::true_type {};


struct ValidObject1 { int data; };
struct ValidObject2 { int method() {}; };

struct InvalidObject1 { int data_; };
struct InvalidObject2 { int method_() {}; };


static_assert(has_data<ValidObject1>::value, "");
static_assert(has_method<ValidObject2>::value, "");

static_assert(!has_data<InvalidObject1>::value, "");
static_assert(!has_method<InvalidObject2>::value, "");

int main() {}

#endif









#ifdef EXAMPLE_07

template<typename T>
class has_isAvailable
{
private:
  template<typename C, typename P>
  static auto test(P * p) -> decltype(std::declval<C>().isAvailable(*p), std::true_type());

  template<typename, typename>
  static std::false_type test(...);

  using type = decltype(test<T, std::nullptr_t>(nullptr));

public:
  static const bool value = std::is_same<std::true_type, type>::value;
};


struct ValidObject
{
  template<typename Where>
  bool isAvailable(Where const &where) const { return false; }
};

struct InvalidObject {};


static_assert(has_isAvailable<ValidObject>::value, "");
static_assert(!has_isAvailable<InvalidObject>::value, "");

int main() {}

#endif









#ifdef EXAMPLE_08

template<class F> struct FunctionTraits;


template<class R, class... Args>
struct FunctionTraits<R(Args...)>
{
  using return_type = R;

  static constexpr std::size_t arity() { return sizeof...(Args); }

  template<std::size_t N> struct argument
  {
    static_assert(N < arity(), "");
    using type = typename std::tuple_element<N, std::tuple<Args...> >::type;
  };
};


template<class C, class R, class... Args>
struct FunctionTraits<R(C::*)(Args...)> : public FunctionTraits<R(C&, Args...)>
{
  using class_type = C;

  template<R(C::* method)(Args...)>
  static R make_ptr(void * const object, Args... args)
  {
    return (static_cast<C *>(object)->*method)(args...);
  }
};


struct Math
{
  int pow2(double v) { return v * v; }
};


int main()
{
  Math math;

  using FT = FunctionTraits<decltype(&Math::pow2)>;
  using Pow2 = int(*)(void *, double);
  Pow2 any_pow2 = &FT::template make_ptr<&Math::pow2>;

  void * any_math = &math;

  auto result = (*any_pow2)(any_math, double(2));
  std::cout << result << std::endl;
}

#endif








#ifdef EXAMPLE_09

template<int value>
int eval()
{
  return value * value;
}


template<std::size_t... index>
constexpr
std::array<typename std::add_pointer<decltype(eval<0>)>::type, sizeof...(index)>
make_eval_list(std::index_sequence<index...>)
{
  return { eval<index>... };
}


int main()
{
  constexpr auto eval_fn =
    make_eval_list(std::make_index_sequence<3>{});

  int dim;
  dim = 2;

  int result = eval_fn[dim]();

  std::cout << result << std::endl;
}

#endif








#ifdef EXAMPLE_10

// suffix the parameter so it does not clash with
// any real class called Variable
class CompositeVector<typename Variable_t, int size_v>
{
public:
  // remove the suffix when forwarding;
  // no risk of name clash because it is scoped
  using Variable = Variable_t;
  
  // do not have to use a function but often clearer
  // to do so
  static constexpr int size() { return size_v; }
};

int main() {}
#endif








#ifdef EXAMPLE_11

// *** header file ***

// forward declared since it is only referenced
class Continuum;

// the vast majority of this class can be refactored
// without impacting any other code
// since almost all of it exclusively resides in the
// cpp file
class Equation
{
public:
  // public API is an absolute minimum
  // and only contains declarations
  Equation(Continuum const &continuum);
  void evaluate(Domain const &domain) const;

private:
  // private helper classes do not need to
  // be visible, but for their names
  class Properties;
  class Initialize;
  class Evaluate;
  class Finalize;

  Continuum const &_continuum;
};


// *** cpp file ***

// all of the implementation must be contained within
// a single cpp file.
// this helps keeping classes down to sensible sizes
#include "Continuum.h"
#include "Domain.h"
#include "Equation.h"

namespace
{
  /// helper functions
}

class Equation::Properties
{
public:
  Properties(Equation const &outer)
    : _outer(outer)
  {}

  Equation const &outer() const { return _outer; }
  Continuum const &continuum() const { return _outer._continuum; }

private:
  Equation const &_outer;
};

class Equation::Initialize ...

class Equation::Evaluate
  // inherits all the properties so they are immediately accessible
  : public Equation::Properties
{
public:
  Evaluate(Equation const &outer)
    : Properties(outer)
  {}

  // no need to split declaration and definition
  void evaluate() const
  {
    // ...
  }
};

class Equation::Finalize ...


void
Equation::
evaluate(Domain const &domain) const
{
  // top level is a flat sequence of operations
  Equation::Initialize(_outer).evaluate(domain);
  Equation::Evaluate(_outer).evaluate(domain);
  Equation::Finalize(_outer).evaluate(domain);
}

#endif








#ifdef EXAMPLE_12

class Condition {};

class InletCondition: public Condition
{
public:
  int eval() { return 100; }
};

class WallCondition: public Condition
{
public:
  int eval() { return 200; }
};


int eval(Condition &condition)
{
  // compile-time cast: can the referenced object
  // be cast to a WallCondition?
  // This will compile and run but is not meaningful:
  // InletCondition is not a WallCondition
  WallCondition &wc =
    static_cast<WallCondition &>(condition);
  return wc.eval();
}


int main()
{
  InletCondition wc;
  int id = eval(wc);
  return sizeof(wc) * id;
}

#endif








#ifdef EXAMPLE_13

class Var
{
public:
  virtual int id() const { return 0; }
};

class Pressure: public Var
{
public:
  int id() const { return 100; }
};


class Equation
{
public:
  Equation(Var const &var)
    // The actual type of var is not necessarily
    // identical to the type of _var.
    : _var(var)
  {}

  int eval() const
  {
    return _var.id();
  }

private:
  // Owned member, not a reference to
  // some external object.
  // This should be a reference.
  Var const _var;
};


int main()
{
  Pressure pres;
  Equation eqn(pres);
  return eqn.eval();
}

#endif
