#include "iostream"

class Fnvla
{
  std::size_t h = 14695981039346656037u;

public:
  using result_type = std::size_t;

  void operator()(void const * key, std::size_t len) noexcept
  {
    unsigned char const * p = static_cast<unsigned char const *>(key);
    unsigned char const * const e = p + len;

    for (; p < e; ++p)
      h = (h ^ *p) * 1099511628211u;
  }

  explicit operator result_type() noexcept { return h; }
};


int main()
{
  std::string key("key");
  Fnvla fnvla;
  fnvla(key.c_str(), key.length());
  std::cout << Fnvla::result_type(fnvla) << std::endl;
}
