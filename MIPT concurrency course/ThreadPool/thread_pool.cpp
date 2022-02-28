#include <tp/thread_pool.hpp>

#include <cassert>

#include <twist/util/thread_local.hpp>

namespace tp {

////////////////////////////////////////////////////////////////////////////////

static twist::util::ThreadLocalPtr<ThreadPool> pool;

////////////////////////////////////////////////////////////////////////////////

ThreadPool::ThreadPool(size_t workers) {
  LaunchWorkers(workers);
}

ThreadPool::~ThreadPool() {
  assert(worker_threads_.empty());
}

void ThreadPool::Submit(Task task) {
  executing_tasks_counter_.Inc();
  task_queue_.Put(std::move(task));
}

void ThreadPool::WaitIdle() {
  executing_tasks_counter_.Wait();
}

void ThreadPool::Stop() {
  task_queue_.Cancel();
  for (auto& worker : worker_threads_) {
    worker.join();
  }
  worker_threads_.clear();
}

ThreadPool* ThreadPool::Current() {
  return pool;
}

void ThreadPool::LaunchWorkers(size_t workers) {
  for (size_t i = 0; i < workers; ++i) {
    worker_threads_.emplace_back([this] {
      pool = this;
      Work();
    });
  }
}

void ThreadPool::Work() {
  while (true) {
    auto task = task_queue_.Take();

    if (task.has_value()) {
      try {
        task.value()();
      } catch (...) {
      }

      executing_tasks_counter_.Dec();
    } else {
      break;
    }
  }
}

}  // namespace tp

