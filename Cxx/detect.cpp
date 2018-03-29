// alternative to std::enable_if using type dispatch

template<typename E>
struct Eval
{
private:
  template<typename E_>
  static typename E_::type detect(Special<E_> const &);

  static E detect(...);  

public:
  using type = decltype(detect(std::declval<std::decay_t<E>>()));
};
