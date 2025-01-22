namespace SimpleMeta
{
  template<typename F> struct function_traits;


  template<typename R, typename... Args>
  struct function_traits<R(Args...)>
  {
    using return_type = R;

    static constexpr std::size_t arity() { return sizeof...(Args); }

    template<std::size_t N>
    struct argument
    {
      static_assert(N < arity(), "");
      using type = typename std::tuple_element<N, std::tuple<Args...> >::type;
    };
  };


  template<typename R, typename... Args>
  struct function_traits<R(*)(Args...)>
    : public function_traits<R(Args...)>
  {};


  template<typename C, typename R, typename... Args>
  struct function_traits<R(C::*)(Args...)>
    : public function_traits<R(C&, Args...)>
  {
    using class_type = C;

    template<R(C::* method)(Args...)>
    inline static R function(void * const object, Args... args)
    {
      return (static_cast<C *>(object)->*method)(args...);
    }
  };


  template<typename C, typename R, typename... Args>
  struct function_traits<R(C::*)(Args...) const>
    : public function_traits<R(C&, Args...)>
  {
    using class_type = C;

    template<R(C::* method)(Args...) const>
    inline static R function(void const * const object, Args... args)
    {
      return (static_cast<C const *>(object)->*method)(args...);
    }
  };


  template<typename C, typename R, typename... Args>
  struct function_traits<R(C::*)(Args...) noexcept>
    : public function_traits<R(C&, Args...)>
  {
    using class_type = C;

    template<R(C::* method)(Args...) noexcept>
    inline static R function(void * const object, Args... args) noexcept
    {
      return (static_cast<C *>(object)->*method)(args...);
    }
  };


  template<typename C, typename R, typename... Args>
  struct function_traits<R(C::*)(Args...) const noexcept>
    : public function_traits<R(C&, Args...)>
  {
    using class_type = C;

    template<R(C::* method)(Args...) const noexcept>
    inline static R function(void const * const object, Args... args) noexcept
    {
      return (static_cast<C const *>(object)->*method)(args...);
    }
  };


  template<typename C, typename R>
  struct function_traits<R(C::*)>
    : public function_traits<R(C&)>
  {
    using class_type = C;
  };


  template<typename F, F f>
  struct function_binding
    : public function_traits<F>
  {
    static constexpr F function_address() noexcept { return f; }
  };
}


struct Engine
{
  inline void at1(int i, float &rhs) {}
  inline void at2(int i, float &rhs) const {}
  inline void at3(int i, float &rhs) noexcept {}
  inline void at4(int i, float &rhs) const noexcept {}
};


int main()
{
  using F = SimpleMeta::function_traits<decltype(&std::decay_t<Engine>::at4)>;
  using A1 = std::decay_t<typename F::template argument<1>::type>;
  using A2 = std::decay_t<typename F::template argument<2>::type>;
  auto arg1 = A1{};
  auto arg2 = A2{};
  Engine{}.at4(arg1, arg2);
}
