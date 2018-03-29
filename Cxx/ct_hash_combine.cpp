
constexpr uint64_t hash(uint64_t const &v)
{
   return v * uint64_t(104723);
}

constexpr uint64_t hash_combine(uint64_t const &seed, uint64_t const &v)
{
  return seed ^ (hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

template<typename... Args>
constexpr uint64_t hash_combine(uint64_t const &seed, uint64_t const &v, Args... args)
{
   return hash_combine(hash_combine(seed, args...), v);
}
