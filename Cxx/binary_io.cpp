#include <cstdint>
#include <limits>
#include <fstream>
#include <iostream>


int main()
{
  std::string const filename("test.bin");
  using numeric_t = uint64_t;

  {
    numeric_t data(std::numeric_limits<numeric_t>::max());
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<char const *>(&data), sizeof(data));
    std::cout << "write: " << data << std::endl;
  }

  {
    numeric_t data(0);
    std::ifstream file(filename, std::ios::binary);
    file.read(reinterpret_cast<char *>(&data), sizeof(data));
    std::cout << "read:  " << data << std::endl;
  }
}
