template<std::size_t pvalue, std::size_t base>
struct RoundUp
{
  static auto constexpr remainder_ = pvalue % base;
  static auto constexpr enable_ = (remainder_ > 0)? 1: 0;
  static auto constexpr value = pvalue + enable_ * (base - remainder_);
};
