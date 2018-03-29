#include <iostream>
#include <tuple>


int main()
{
  std::tuple<int, int, int> t{5,6,7};

  std::cout << "ts " << sizeof(t) << std::endl;
  std::cout << "te " << sizeof(std::get<0>(t)) << std::endl;
  std::cout << "&t " << &t << std::endl;
  std::cout << "t2 " << &std::get<2>(t) << std::endl;
  std::cout << "t1 " << &std::get<1>(t) << std::endl;
  std::cout << "t0 " << &std::get<0>(t) << std::endl;


  char * uN = reinterpret_cast<char *>(&t);
  char * u0 = uN + sizeof(t) - sizeof(std::get<0>(t));
  char * u1 = u0 - sizeof(std::get<0>(t));

  std::cout << "u0  " << (int)(*u0) << std::endl;
  std::cout << "u1  " << (int)(*u1) << std::endl;
  std::cout << "uN  " << (int)(*uN) << std::endl;


  char * v0 = reinterpret_cast<char *>(&t) + sizeof(t) - sizeof(std::get<0>(t));
  char * v1 = v0 - sizeof(std::get<0>(t))*1;
  char * v2 = v0 - sizeof(std::get<0>(t))*2;

  std::cout << "v0  " << (int)(*v0) << std::endl;
  std::cout << "v1  " << (int)(*v1) << std::endl;
  std::cout << "v2  " << (int)(*v2) << std::endl;
}

