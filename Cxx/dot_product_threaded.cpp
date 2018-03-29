// compile with: -pthread

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <chrono>
#include <atomic>
#include <mutex>


using numeric_t = double;


std::vector<int> bounds(int const parts, int const size)
{
  std::vector<int> result(parts + 1);
  int const delta = size / parts;
  int const reminder = size % parts;
  int lbnd = 0;
  int ubnd = 0;
  result[0] = lbnd;

  for (size_t i = 0; i < parts; ++i)
  {
    ubnd = lbnd + delta;
    if (i == parts - 1)
      ubnd += reminder;

    result[i+1] = ubnd;
    lbnd = ubnd;
  }

  return result;
}


class DotProduct
{
public:
  DotProduct(std::vector<int> const &limits,
             std::vector<numeric_t> const &v1,
             std::vector<numeric_t> const &v2)
    : limits(limits)
    , v1(v1)
    , v2(v2)
  {}

  numeric_t operator()() const
  {
    std::vector<std::thread> threads(limits.size() - 1);
    std::atomic<numeric_t> result(0);

    for (size_t i = 0; i < threads.size(); ++i)
      threads[i] = std::thread(&DotProduct::eval, *this, i, std::ref(result));

    for(auto &thread: threads)
      thread.join();

    return result;
  }

private:
  void eval(int const offset,
            std::atomic<numeric_t> &result) const
  {
    numeric_t partial = 0;

    for(size_t i = limits[offset]; i < limits[offset+1]; ++i)
      partial += v1[i] * v2[i];

    result = result + partial;
  }

  std::vector<int> const &limits;
  std::vector<numeric_t> const &v1;
  std::vector<numeric_t> const &v2;
};


int main()
{
  int const nr_elements = 100000000;
  int const nr_threads = 8;
  std::vector<int> const limits = bounds(nr_threads, nr_elements);
  std::vector<numeric_t> const v1(nr_elements, 1);
  std::vector<numeric_t> const v2(nr_elements, 2);

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  DotProduct const dotProduct(limits, v1, v2);
  numeric_t const result = dotProduct();

  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "actual time: " << elapsed_seconds.count() << "s" << std::endl;

  std::cout << "result: " << result << std::endl;
}
