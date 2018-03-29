// -*- C++ -*-

#ifndef _ScopeGuard_h_
#define _ScopeGuard_h_

// base on:
// http://cppsecrets.blogspot.co.uk/2013/11/ds-scope-statement-in-c.html

#include <exception>
#include <utility>
#include <type_traits>


template<typename F, typename CleanupPolicy, bool IsActive = true>
class ScopeGuard
  : CleanupPolicy
  , CleanupPolicy::Installer
{
  using CleanupPolicy::cleanup;
  using CleanupPolicy::Installer::install;

  typename std::remove_reference<F>::type _f;

public:
  ScopeGuard(F&& f)
    : _f(std::forward<F>(f)) {
    if (IsActive)
      install();
    else
      _f();
  }

  ~ScopeGuard() {
    if (IsActive)
      cleanup(_f);
  }
};


struct UncheckedInstallPolicy {
  void install() {}
};


struct CheckedInstallPolicy {
  void install() {
    if (std::uncaught_exception()) {
      std::terminate();
    }
  }
};


struct ExitPolicy {
  typedef UncheckedInstallPolicy Installer;

  template<typename F>
  void cleanup(F& f) {
    f();
  }
};

typedef ExitPolicy exitPolicy;


struct FailurePolicy {
  typedef CheckedInstallPolicy Installer;

  template<typename F>
  void cleanup(F& f) {
    if (std::uncaught_exception()) {
      f();
    }
  }
};

typedef FailurePolicy failurePolicy;


struct SuccessPolicy {
  typedef CheckedInstallPolicy Installer;

  template<typename F>
  void cleanup(F& f) {
    if (!std::uncaught_exception()) {
      f();
    }
  }
};

typedef SuccessPolicy successPolicy;


template<typename CleanupPolicy, bool IsActive = true>
struct ScopeGuardBuilder {};


template<typename F, typename CleanupPolicy, bool IsActive>
ScopeGuard<F, CleanupPolicy, IsActive>
operator+(ScopeGuardBuilder<CleanupPolicy, IsActive>, F&& f)
{
  return ScopeGuard<F, CleanupPolicy, IsActive>(std::forward<F>(f));
}


#define PasteTokensImpl(a, b) a ## b
#define PasteTokens(a, b) PasteTokensImpl(a, b)

#define scopeOverload(_1, _2, name, ...) name
#define scope(...) scopeOverload(__VA_ARGS__, scope2)(__VA_ARGS__)

#define scope1(condition)                                               \
  auto PasteTokens(_scopeGuard, __LINE__) =                             \
    ScopeGuardBuilder<PasteTokens(condition, Policy), true>() + [&]

#define scope2(condition, IsActive)                                     \
  auto PasteTokens(_scopeGuard, __LINE__) =                             \
    ScopeGuardBuilder<PasteTokens(condition, Policy), IsActive>() + [&]

#endif // _ScopeGuard_h_



#include <iostream>

struct Primal {};
struct Tangent {};
struct Adjoint {};

template<typename Mode> void initialize()
{
  std::cout << "initialize" << std::endl;
}

template<typename Mode> void accumulate()
{
  std::cout << "accumulate" << std::endl;
}

template<typename Mode> void normalize()
{
  std::cout << "normalize" << std::endl;
}

template<typename Mode> void evaluate()
{
  bool const activate = std::is_same<Mode, Adjoint>::value;
  scope(exit, activate) { initialize<Mode>(); };
  scope(exit, activate) { accumulate<Mode>(); };
  scope(exit, activate) { normalize<Mode>(); };
}

int main()
{

  evaluate<Primal>();

  evaluate<Tangent>();

  evaluate<Adjoint>();
}
