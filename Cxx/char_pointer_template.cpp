#include <iostream>

template<char const * ptr>
struct CharPointer
{
  CharPointer(): name(ptr) {}
  char const * name;
};

struct FileMacro
{
 static char const name[];
};

decltype(FileMacro::name) FileMacro::name = __FILE__;

int main()
{
  CharPointer<FileMacro::name> filename;
  std::cout << std::string(filename.name) << std::endl;
}
