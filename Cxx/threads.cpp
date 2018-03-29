// compile with: -D_GLIBCXX_USE_NANOSLEEP -pthread

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>


void evaluate(int duration)
{
  std::this_thread::sleep_for(std::chrono::seconds(duration));
}


int main()
{
  int const nThreads = 4;
  std::vector<std::thread> threads(nThreads);

  int const duration = 1;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  for (std::thread &thread: threads)
    {
      thread = std::thread(evaluate, duration);
    }

  for (std::thread &thread: threads)
    {
      thread.join();
    }

  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  std::cout << "expected time: " << duration << "s" << std::endl;
  std::cout << "actual time: " << elapsed_seconds.count() << "s" << std::endl;
}
