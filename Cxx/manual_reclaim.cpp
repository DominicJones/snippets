#include <iostream>
#include <array>
#include <vector>
#include <cmath>
#include <cstddef>
#include <cstring>


template <std::size_t N>
class ManualReclaimMemPool
{
private:
  enum class Location { STACK, HEAP, };

public:
  ManualReclaimMemPool()
    : _loc(Location::STACK)
    , _pos(0)
    , _bytesAllocated(0)
    , _bytesDeallocated(0)
  {}

  ManualReclaimMemPool(ManualReclaimMemPool const &) = delete;
  ManualReclaimMemPool &operator=(ManualReclaimMemPool const &) = delete;

  char * allocate(std::size_t n)
  {
    std::size_t const newSize(_pos + n);
    char * startAddress(nullptr);

    switch (_loc) {
    case Location::STACK:
    {
      if (_stack.size() >= newSize)
      {
        startAddress = &_stack[_pos];
      }
      else
      {
        migrateToHeap(newSize);
        startAddress = &_heap[_pos];
      }
    } break;
    case Location::HEAP:
    {
      if (_heap.size() < newSize)
      {
        resizeHeap(newSize);
      }
      startAddress = &_heap[_pos];
    } break;
    }

    _pos += n;
    _bytesAllocated += n;
    return startAddress;
  }

  void deallocate(char * p, std::size_t n)
  {
    _bytesDeallocated += n;
  }

  bool reclaim()
  {
    bool success(false);

    if (bytesAllocated() == bytesDeallocated())
    {
      _pos = 0;
      _bytesAllocated = 0;
      _bytesDeallocated = 0;
      success = true;
    }

    return success;
  }

  bool clear()
  {
    bool success(false);

    if (reclaim())
    {
      _heap.clear();
      _loc = Location::STACK;
      success = true;
    }

    return success;
  }

  void status() const
  {
    std::cout << "---" << std::endl;
    std::cout << " stack size = " << stackSize() << std::endl;
    std::cout << " heap  size = " << heapSize() << std::endl;
    std::cout << " bytes   allocated = " << bytesAllocated() << std::endl;
    std::cout << " bytes deallocated = " << bytesDeallocated() << std::endl;
  }

  inline std::size_t stackSize() const { return _stack.size(); }
  inline std::size_t heapSize() const { return _heap.size(); }

  inline std::size_t bytesAllocated() const { return _bytesAllocated; }
  inline std::size_t bytesDeallocated() const { return _bytesDeallocated; }

private:
  void migrateToHeap(std::size_t n)
  {
    _heap.resize(nearestSize(n, N * 2));
    std::memcpy(_heap.data(), _stack.data(), _pos);
    _loc = Location::HEAP;
  }

  void resizeHeap(std::size_t n)
  {
    std::vector<char> tempHeap;
    tempHeap.resize(_pos);
    std::memcpy(tempHeap.data(), _heap.data(), _pos);

    _heap.resize(nearestSize(n, _heap.size()));
    std::memcpy(_heap.data(), tempHeap.data(), _pos);
  }

  std::size_t nearestSize(std::size_t n, std::size_t n0) const
  {
    std::size_t size(n0);

    for(;;)
      if (n > size)
        size *= 2;
      else
        break;

    return size;
  }

  std::array<char, N> _stack;
  std::vector<char> _heap;
  Location _loc;
  std::size_t _pos;
  std::size_t _bytesAllocated;
  std::size_t _bytesDeallocated;
};




template <class T, std::size_t N>
class ManualReclaimAllocator
{
public:
  typedef T value_type;

  template <class U> struct rebind
  {
    typedef ManualReclaimAllocator<U, N> other;
  };

  explicit ManualReclaimAllocator(ManualReclaimMemPool<N> &memPool)
    : _memPool(memPool)
  {}

  template <class U>
  ManualReclaimAllocator(const ManualReclaimAllocator<U, N> &memPool)
    : _memPool(memPool._memPool)
  {}

  ManualReclaimAllocator(ManualReclaimAllocator const &) = default;
  ManualReclaimAllocator &operator=(ManualReclaimAllocator const &) = delete;

  T * allocate(std::size_t n)
  {
    return reinterpret_cast<T*>(_memPool.allocate(n * sizeof(T)));
  }

  void deallocate(T * p, std::size_t n)
  {
    _memPool.deallocate(reinterpret_cast<char*>(p), n * sizeof(T));
  }

  template <class T1, std::size_t N1, class U, std::size_t M>
  friend
  bool
  operator==(ManualReclaimAllocator<T1, N1> const &x, ManualReclaimAllocator<U, M> const &y);

  template <class U, std::size_t M> friend class ManualReclaimAllocator;

private:
  ManualReclaimMemPool<N> &_memPool;
};


template <class T, std::size_t N, class U, std::size_t M>
bool
operator==(ManualReclaimAllocator<T, N> const &x, ManualReclaimAllocator<U, M> const &y)
{
  return (N == M) && (&x._memPool == &y._memPool);
}


template <class T, std::size_t N, class U, std::size_t M>
bool
operator!=(ManualReclaimAllocator<T, N> const &x, ManualReclaimAllocator<U, M> const &y)
{
  return !(x == y);
}




template<class T>
inline constexpr T pow(const T base, unsigned const exponent)
{
  return (exponent == 0)? 1:
    (exponent % 2 == 0)? pow(base, exponent/2) * pow(base, exponent / 2):
    base * pow(base, (exponent - 1) / 2) * pow(base, (exponent - 1) / 2);
}


template <class T, std::size_t N>
using Vector = std::vector<T, ManualReclaimAllocator<T, N> >;


int main()
{
  std::size_t const n(pow(2, 9));
  std::size_t const N(pow(2, 8));
  ManualReclaimMemPool<N> memPool;
  memPool.status();

  {
    char * block1 = memPool.allocate(n);
    memPool.status();
    char * block2 = memPool.allocate(n);
    memPool.status();
    char * block3 = memPool.allocate(n);
    memPool.status();

    memPool.reclaim();
    memPool.status();

    memPool.deallocate(block3, n);
    memPool.status();
    memPool.deallocate(block2, n);
    memPool.status();
    memPool.deallocate(block1, n);
    memPool.status();

    memPool.reclaim();
    memPool.status();

    memPool.clear();
    memPool.status();
  }

  {
    ManualReclaimAllocator<char, N> allocator(memPool);
    Vector<char, N> v(allocator);

    v.resize(N);
    memPool.status();

    for (auto && e : v)
      e = 123;

    Vector<char, N> v2 = std::move(v);
    v = v2;
  }
}
