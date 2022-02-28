#pragma once

#include <twist/stdlike/atomic.hpp>

#include <cstdlib>

#include "wait_queue.hpp"

namespace stdlike {

class Mutex {
 public:
  void Lock() {
    while (locked_.exchange(1) == 1) {
      waiting_.fetch_add(1);
      locked_.FutexWait(1);
      waiting_.fetch_sub(1);
    }
  }

  void Unlock() {
    locked_.store(0);
    if (waiting_.load() > 0) {
      locked_.FutexWakeOne();
    }
  }

  Mutex() : locked_(0), waiting_(0) {
  }

 private:
  twist::stdlike::atomic<uint32_t> locked_;
  twist::stdlike::atomic<uint32_t> waiting_;
};

}  // namespace stdlike

