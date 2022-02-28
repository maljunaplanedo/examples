#pragma once

#include <tp/blocking_queue.hpp>
#include <tp/blocking_counter.hpp>
#include <tp/task.hpp>

#include <cstdint>
#include <list>

#include <twist/stdlike/thread.hpp>

namespace tp {

// Fixed-size pool of worker threads

class ThreadPool {
 public:
  explicit ThreadPool(size_t workers);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Schedules task for execution in one of the worker threads
  void Submit(Task task);

  // Waits until outstanding work count has reached zero
  void WaitIdle();

  // Stops the worker threads as soon as possible
  // Pending tasks will be discarded
  void Stop();

  // Locates current thread pool from worker thread
  static ThreadPool* Current();

 private:
  void LaunchWorkers(size_t workers);
  void Work();

 private:
  UnboundedBlockingQueue<Task> task_queue_;
  std::list<twist::stdlike::thread> worker_threads_;
  detail::BlockingZeroWaitMPMCCounter executing_tasks_counter_;

  // Worker threads, task queue, etc
};

inline ThreadPool* Current() {
  return ThreadPool::Current();
}

}  // namespace tp

