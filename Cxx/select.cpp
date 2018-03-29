#include <tuple>
#include <type_traits>
#include <memory>
#include <iostream>


template<typename VariantsBase, typename... Variants>
class Select
{
private:
  using VariantsTuple = std::tuple<Variants...>;

public:
  template<typename T>
  Select(T const &index)
    : _index(static_cast<typename std::underlying_type<T>::type>(index))
  {}

  template<typename... Args>
  std::unique_ptr<VariantsBase> make(Args &&... args) const
  {
    constexpr auto size(std::tuple_size<VariantsTuple>::value);
    return sequence(std::make_index_sequence<size>{}, std::forward<Args>(args)...);
  }

private:
  template<std::size_t... index, typename... Args>
  typename std::enable_if<bool(std::index_sequence<index...>::size() != 0), std::unique_ptr<VariantsBase> >::type
  sequence(std::index_sequence<index...>, Args &&... args) const
  {
    constexpr auto size(std::index_sequence<index...>::size());
    using Variant_i = typename std::tuple_element<size - 1, VariantsTuple>::type;
    static_assert(std::is_base_of<VariantsBase, Variant_i>::value, "");

    if (this->_index == size - 1)
      return std::make_unique<Variant_i>(std::forward<Args>(args)...);
    else
      return sequence(std::make_index_sequence<size - 1>{}, std::forward<Args>(args)...);
  }

  template<std::size_t... index, typename... Args>
  typename std::enable_if<bool(std::index_sequence<index...>::size() == 0), std::unique_ptr<VariantsBase> >::type
  sequence(std::index_sequence<index...>, Args &&... args) const
  {
    return nullptr;
  }

private:
  std::size_t const _index;
};


enum class Mode { PRIMAL = 0, TANGENT, ADJOINT, };

struct TermBase {
  virtual ~TermBase() = default;
  virtual int evaluate() const = 0;
};

struct PrimalTerm : TermBase {
  PrimalTerm(int, double) {}
  int evaluate() const final { return 0; }
};

struct TangentTerm : TermBase {
  TangentTerm(int, double) {}
  int evaluate() const final { return 1; }
};

struct AdjointTerm : TermBase {
  AdjointTerm(int, double) {}
  int evaluate() const final { return 2; }
};


int main()
{
  auto const term(Select<TermBase, PrimalTerm, TangentTerm, AdjointTerm>(Mode::TANGENT).make(1, 2.0));
  std::cout << term->evaluate() << std::endl;
}
