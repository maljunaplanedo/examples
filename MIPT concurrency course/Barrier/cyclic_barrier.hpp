#pragma once

#include <twist/stdlike/mutex.hpp>
#include <twist/stdlike/condition_variable.hpp>

// std::lock_guard, std::unique_lock
#include <mutex>
#include <cstdint>

namespace solutions {

// CyclicBarrier allows a set of threads to all wait for each other
// to reach a common barrier point

// The barrier is called cyclic because
// it can be re-used after the waiting threads are released.

class CyclicBarrier {
 public:
  explicit CyclicBarrier(size_t participants)
      : participants_(participants), target_(participants), arrived_(0) {
  }

  // Blocks until all participants have invoked Arrive()
  void Arrive() {
    std::unique_lock guard(mutex_);

    size_t this_thread_target = target_;
    ++arrived_;

    if (arrived_ == this_thread_target) {
      target_ += participants_;
      target_reached_.notify_all();
    } else {
      while (arrived_ < this_thread_target) {
        target_reached_.wait(guard);
      }
    }
  }

 private:
  const size_t participants_;
  size_t target_;   // guarded by mutex_
  size_t arrived_;  // guarded by mutex_

  twist::stdlike::mutex mutex_;
  twist::stdlike::condition_variable target_reached_;
};

}  // namespace solutions

