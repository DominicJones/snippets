#include <type_traits>
#include <new>


namespace OpaqueObjectManager_impl
{
  template<std::size_t stackSize, std::size_t stateAlign>
  union alignas(stateAlign) State
  {
    void * heap;
    unsigned char stack[stackSize];
  };


  template<typename Object, std::size_t stackSize, std::size_t stateAlign>
  class OpaqueObjectHelper
  {
  public:
    using State_t = OpaqueObjectManager_impl::State<stackSize, stateAlign>;

    inline static void
    initialize(State_t &state, Object const &object) noexcept
    {
      initialize(state, object, useStack());
    }

    inline static void
    clone(State_t const &src, State_t &dest) noexcept
    {
      clone(src, dest, useStack());
    }

    inline static void
    destroy(State_t &state) noexcept
    {
      destroy(state, useStack());
    }

    inline static void *
    address(State_t &state) noexcept
    {
      return address(state, useStack());
    }

  private:
    inline static auto constexpr
    useStack() noexcept
    {
      bool constexpr useStack_{sizeof(Object) <= sizeof(State_t)};
      return std::integral_constant<bool, useStack_>{};
    }

    inline static void
    initialize(State_t &state, Object const &object, std::true_type) noexcept
    {
      ::new(&state.stack[0]) Object(object);
    }

    inline static void
    initialize(State_t &state, Object const &object, std::false_type) noexcept
    {
      state.heap = new Object(object);
    }

    inline static void
    clone(State_t const &src, State_t &dest, std::true_type) noexcept
    {
      ::new(&dest.stack[0]) Object(*((Object*)&src.stack[0]));
    }

    inline static void
    clone(State_t const &src, State_t &dest, std::false_type) noexcept
    {
      dest.heap = new Object(*((Object*)src.heap));
    }

    inline static void
    destroy(State_t &state, std::true_type) noexcept
    {
      Object * _object = reinterpret_cast<Object *>(address(state));
      _object->~Object();
    }

    inline static void
    destroy(State_t &state, std::false_type) noexcept
    {
      Object * _object = reinterpret_cast<Object *>(address(state));
      delete _object;
    }

    inline static void *
    address(State_t &state, std::true_type) noexcept
    {
      return &state.stack[0];
    }

    inline static void *
    address(State_t &state, std::false_type) noexcept
    {
      return state.heap;
    }
  };
}


template<std::size_t stackSize, std::size_t stateAlign = alignof(int)>
class OpaqueObjectManager
{
private:
  using State_t = OpaqueObjectManager_impl::State<stackSize, stateAlign>;
  using Clone  = void (*)(State_t const &, State_t &);
  using Destroy = void (*)(State_t &);
  using Address = void * (*)(State_t &);

public:
  template<typename Object>
  inline explicit OpaqueObjectManager(Object const &object) noexcept
  {
    initialize(object);
  }

  inline ~OpaqueObjectManager() noexcept { _destroy(_state); }

  inline OpaqueObjectManager(OpaqueObjectManager const &object) noexcept
  {
    clone(object);
  }

  inline OpaqueObjectManager &operator=(OpaqueObjectManager const &object) noexcept
  {
    _destroy(_state);
    clone(object);
    return *this;
  }

  template<typename Object>
  inline OpaqueObjectManager &operator=(Object const &object) noexcept
  {
    _destroy(_state);
    initialize(object);
    return *this;
  }

  inline void * address() noexcept
  {
    return _address(_state);
  }

private:
  inline void clone(OpaqueObjectManager const &object) noexcept
  {
    object._clone(object._state, _state);
    _clone   = object._clone;
    _destroy = object._destroy;
    _address  = object._address;
  }

  template<typename Object>
  inline void initialize(Object const &object) noexcept
  {
    OpaqueObjectManager_impl::OpaqueObjectHelper<Object, stackSize, stateAlign>::initialize(_state, object);
    _clone   = &OpaqueObjectManager_impl::OpaqueObjectHelper<Object, stackSize, stateAlign>::clone;
    _destroy = &OpaqueObjectManager_impl::OpaqueObjectHelper<Object, stackSize, stateAlign>::destroy;
    _address = &OpaqueObjectManager_impl::OpaqueObjectHelper<Object, stackSize, stateAlign>::address;
  }

  State_t _state;
  Clone _clone;
  Destroy _destroy;
  Address _address;
};




#include <iostream>
#include <array>


template<typename T>
using AdjointFunction = void(*)(void const * const object, T const &value);


template<class T>
struct Expression
{
public:
  void adjoint(T const &v) const
  {
    std::cout << __func__ << std::endl;
  }

private:
  std::array<unsigned char, 128> _state;
};


template<typename T>
inline void
Expression_adjoint(void const * const expr, T const &value)
{
  decltype(auto) expr_cast = reinterpret_cast<Expression<T> const *>(expr);
  expr_cast->adjoint(value);
}


int main()
{
  double value = 1.0;
  Expression<double> expr;
  auto object = OpaqueObjectManager<128>{expr};
  auto function = AdjointFunction<double>{&Expression_adjoint<double>};
  (*function)(object.address(), value);
}
