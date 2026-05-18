#include "flower/thread/ThreadPool.h"
#include <iostream>
#include <chrono>

using namespace std;

int main() {
  ThreadPool pool(4);

  vector<future<int>> results;
  for (int i = 0; i < 8; ++i) {
    results.emplace_back(pool.enqueue([i] {
      this_thread::sleep_for(chrono::milliseconds(500));
      return i * i;
    }));
  }

  for (auto &r : results)
    cout << r.get() << endl;

  return 0;
}
