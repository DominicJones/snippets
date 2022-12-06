static std::size_t constexpr ct_sqrt(std::size_t res, std::size_t l, std::size_t r)
{
  if (l == r)
  {
    return r;
  }
  else
  {
    auto const mid = (r + l) / 2;

    if (mid * mid >= res)
      return ct_sqrt(res, l, mid);
    else
      return ct_sqrt(res, mid + 1, r);
  }
}


static std::size_t constexpr ct_sqrt(std::size_t res)
{
  return ct_sqrt(res, 1, res);
}
