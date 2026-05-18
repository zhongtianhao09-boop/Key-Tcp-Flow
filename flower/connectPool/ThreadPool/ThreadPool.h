#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
  ~ThreadPool();

  template <class F, class... Args>
  auto enqueue(F &&f, Args &&...args)
      -> std::future<typename std::invoke_result_t<F, Args...>>;

private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

  std::mutex queueMutex;
  std::condition_variable cv;
  bool stop = false;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args)
    -> std::future<typename std::invoke_result_t<F, Args...>> {

  using returnType = typename std::invoke_result_t<F, Args...>;

  auto task = std::make_shared<std::packaged_task<returnType()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<returnType> result = task->get_future();

  {
    std::unique_lock<std::mutex> lock(queueMutex);
    if (stop)
      throw std::runtime_error("enqueue on stopped ThreadPool");
    tasks.emplace([task]() { (*task)(); });
  }
  cv.notify_one();
  return result;
}

#endif
