#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include "log.h"

class Barrier
{
  int resistance;
  std::mutex m;
  std::mutex b_mut;
public:
  Barrier(int resistance)
  {
    this->resistance = resistance;
    b_mut.lock();
  }

  int reach()
  {
    m.lock();
    resistance--;
    if (resistance == 0) {
      b_mut.unlock();
    }
    m.unlock();

    b_mut.lock();
    b_mut.unlock();
    return 0;
  }
};

void f(int name, int nap_time, Barrier& b)
{
  log("f, ", name, " local section start");
  std::this_thread::sleep_for(std::chrono::milliseconds(100 * nap_time));
  log("f, ", name, " local section finish");

  log("f, ", name, " BARRIER section start");
  b.reach();
  log("f, ", name, " BARRIER section finish");
}

int main()
{
  int resistance{9};
  Barrier b(resistance);
  int n{30};
  std::vector<std::thread> thrds;

  for (int i = 0; i < n; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    thrds.push_back(std::thread{[i, &b]{ f(i, 10, b); }});
  }

  for (int i = 0; i < n; i++) {
    thrds[i].join();
  }

  log("DONE");
}