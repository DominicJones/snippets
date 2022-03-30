
namespace alt
{
  template<typename T>
  class shared_ptr
  {
  public:
    shared_ptr()
      : _ptr(nullptr)
      , _refs(new int(0))
    {}

    shared_ptr(T * ptr)
      : _ptr(ptr)
      , _refs(new int(1))
    {}

    shared_ptr(shared_ptr const &other)
    {
      this->_ptr = other._ptr;
      this->_refs = other._refs;
      if (other._ptr != nullptr)
        (*this->_refs)++;
    }

    void operator=(shared_ptr const &other)
    {
      reset();
      this->_ptr = other._ptr;
      this->_refs = other._refs;
      if (other._ptr != nullptr)
        (*this->_refs)++;
    }

    shared_ptr(shared_ptr &&other)
    {
      this->_ptr = other._ptr;
      this->_refs = other._refs;
      other._ptr = other._refs = nullptr;
    }

    void operator=(shared_ptr &&other)
    {
      reset();
      this->_ptr = other._ptr;
      this->_refs = other._refs;
      other._ptr = other._refs = nullptr;
    }

    auto use_count() const { return *_refs; }
    T * get() const { return this->_ptr; }
    T * operator->() const { return this->_ptr; }
    T &operator*() const { return *(this->_ptr); }
    ~shared_ptr() { reset(); }

  private:
    void reset()
    {
      (*_refs)--;
      if (*_refs == 0)
      {
        if (_ptr != nullptr) delete _ptr;
        delete _refs;
      }
    }

    T * _ptr = nullptr;
    int * _refs = nullptr;
  };
}



#include <iostream>


struct Box
{
  int length, width, height;
  Box() : length(0), width(0), height(0) {}
};


int main()
{
  {
    alt::shared_ptr<Box> obj;
    std::cout << obj.use_count() << std::endl; // prints 0
    {
      alt::shared_ptr<Box> box1(new Box());
      std::cout << box1.use_count() << std::endl; // prints 1
      {
        alt::shared_ptr<Box> box2(box1);
        std::cout << box1.use_count() << std::endl; // prints 2
        std::cout << box2.use_count() << std::endl; // prints 2
      }
      std::cout << box1.use_count() << std::endl; // prints 1
    }
    std::cout << obj.use_count() << std::endl; // prints 0
  }
}
