/*
  Static initialisation issue

  **

  using a statically initialised map
  in a statically executed function
  causes a segmentation fault

  tested with:
  g++ (Ubuntu/Linaro 4.8.1-10ubuntu8) 4.8.1
*/


/*
  Correspondence from Jonathan Wakely:

  **

  I was looking for the C++ defect
  that gives compilers permission to do instantiation at the end of the
  file, as I mentioned, and it's

  http://open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#993

  I'm still not 100% sure that's the problem in your example, and that
  you haven't found a bug, so will try to confirm that properly!

  **

  having checked some old GCC bugzilla reports and conferred with our
  C++ front-end expert, I think the behaviour you're seeing is indeed
  allowed by the standard (even though that's not very helpful). So you
  cannot rely on static members of class templates being initialized in
  that order. The problem is that the definition of the member:

  template<class num_type, int size>
  std::map<size_t, std::array<num_type, size>> Storage<num_type, size>::_data;

  is not the point of instantiation, so doesn't mean it will be
  initialized before other (non-temploid) objects that come later in the
  file.
*/


#include <cassert>
#include <type_traits>
#include <memory>
#include <array>
#include <map>
#include <limits>


template<class num_type, int size> class Storage
{
public:
  bool hasStorage() const;
  void addStorage();
  void removeStorage();
  num_type operator[](size_t i) const { return _data[key()][i]; }
  num_type &operator[](size_t i) { return _data[key()][i]; }

protected:
  virtual size_t key() const = 0;

private:
  static std::map<size_t, std::array<num_type, size>> _data;
};


template<class num_type, int size>
std::map<size_t, std::array<num_type, size>> Storage<num_type, size>::_data;


template<class num_type, int size> bool Storage<num_type, size>::hasStorage() const
{
  auto it = _data.find(key());
  return it != _data.end();
}


template<class num_type, int size> void Storage<num_type, size>::addStorage()
{
  if (!hasStorage())
    {
      double const sNan = std::numeric_limits<num_type>::signaling_NaN();
      _data[key()] = std::array<num_type, size>{sNan};
    }
}


template<class num_type, int size> void Storage<num_type, size>::removeStorage()
{
  _data.erase(key());
}


class Values: public Storage<double, 2>
{
public:
  size_t key() const { return typeid(decltype(*this)).hash_code(); }
};


bool unittest()
{
  Values values;
  assert(!values.hasStorage());
  values.addStorage();
  assert(values.hasStorage());
  return true;
}


// ISSUE: comment out to avoid segmentation fault
static bool const executeUnittest = unittest();


int main()
{
  assert(unittest());
}
