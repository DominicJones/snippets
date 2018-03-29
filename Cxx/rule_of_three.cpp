#include <iostream>

// A contrived example of the "rule of three":
// copy constructor, copy assignment operator and destructor

class Container {
public:
  Container (int val_ = 1) {
    std::cout << "constructor" << std::endl;
    _data  = new int;
    *_data = val_;
  }

  Container (const Container& other) {
    std::cout << "copy constructor" << std::endl;
    _data  = new int;
    *(this->_data) = *(other._data) + 1;
  }

  Container& operator=(const Container& other) {
    std::cout << "copy assignment operator" << std::endl;
    if (this != &other)
      {
        int* data_cp = new int;
        *(data_cp) = *(other._data) + 1;

        delete _data;

        _data = data_cp;
      }
    return *this;
  }

  ~Container () {
    std::cout << "destructor" << *_data << std::endl;
    delete _data;
  }

private:
  int* _data;
};


int main()
{
  Container obj1;
  Container obj2 = obj1;
  Container obj3(obj2);
  Container obj4;
  obj4 = obj3;
}
