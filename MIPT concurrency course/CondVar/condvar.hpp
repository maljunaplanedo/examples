
#pragma once

#include <twist/stdlike/atomic.hpp>

#include <cstdint>

namespace stdlike {

class CondVar {
 public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    uint32_t old_value = trigger_.load();
    mutex.unlock();
    trigger_.FutexWait(old_value);
    mutex.lock();
  }

  void NotifyOne() {
    trigger_.fetch_add(1);
    trigger_.FutexWakeOne();
  }

  void NotifyAll() {
    trigger_.fetch_add(1);
    trigger_.FutexWakeAll();
  }

 private:
  twist::stdlike::atomic<uint32_t> trigger_{0};
};

}  // namespace stdlike

