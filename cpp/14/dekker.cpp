#include <atomic>
#include <thread>
#include <iostream>

using namespace std;

const int loops = 10;

atomic<bool> wants1{false};
atomic<bool> wants2{false};
atomic<int> turn{0};

void f2()
{
  for (int i = 0; i < loops; i++) {
    wants2 = true;
    while (wants1.load()) {
      if (turn.load() == 1) {
        wants2 = false;
        while (turn.load() == 1) {
        }
        wants2 = true;
      }
    }
    //critical
    cout << "JESTEM 2 W KRYTYCZNEJ\n";

    turn = 1;
    wants2 = false;
  }
}

void f1()
{
  for (int i = 0; i < loops; i++) {
    wants1 = true;
    while (wants2.load()) {
      if (turn.load() == 0) {
        wants1 = false;
        while (turn.load() == 0) {

        }
        wants1 = true;
      }
    }
    //critical
    cout << "JESTEM 1 W KRYTYCZNEJ\n";

    turn = 0;
    wants1 = false;
  }
}

int main()
{
  thread t1{f1};
  thread t2{f2};

  t1.join();
  t2.join();

  cout << "KONIEC\n";
}