#include "map_enum.h"
#include <iostream>


const std::map<std::string, Colour::Options>
Colour::
_map = Colour::createMap();


int main()
{
  std::string const key("Green");
  std::cout << key << " : " << Colour::getOption(key) << std::endl;
}
