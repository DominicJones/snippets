#include <string>
#include <iostream>

int main()
{
  std::string s = "star.flow.ForceReport_11128";
  std::string c = "_";
  auto const split = s.find(c);
  std::string name = s.substr(0, split);
  std::string id = s.substr(split + c.size(), s.size());
  std::cout << name << std::endl;
  std::cout << id << std::endl;
}
