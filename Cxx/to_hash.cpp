#include <iostream>
#include <string>


unsigned int toHash(std::string const &str)
{
  unsigned int hash = 0;

  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
      hash = 65599 * hash + (*it);
    }

  hash = hash ^ (hash >> 16);

  return hash;
}


int main(void)
{
  std::cout << toHash("message") << std::endl;
}
