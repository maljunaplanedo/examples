#pragma once

#include <cassert>
#include <variant>
#include <memory>

#include <twist/stdlike/atomic.hpp>
#include <twist/stdlike/mutex.hpp>

namespace stdlike::detail {

template <typename T>
class Channel {
 public:
  Channel() {
  }

  void PutValue(T value) {
    std::unique_lock guard(mutex_);
    result_.template emplace<1>(std::move(value));
    guard.unlock();
    has_result_.store(1);
    has_result_.FutexWakeAll();
  }

  void PutException(std::exception_ptr ex) {
    std::unique_lock guard(mutex_);
    result_.template emplace<2>(ex);
    guard.unlock();
    has_result_.store(1);
    has_result_.FutexWakeAll();
  }

  T Get() {
    while (has_result_.load() == 0) {
      has_result_.FutexWait(0);
    }

    std::lock_guard guard(mutex_);
    if (result_.index() == 1) {
      return std::move(std::get<1>(result_));
    }
    std::rethrow_exception(std::get<2>(result_));
  }

  // Non-copyable
  Channel(const Channel&) = delete;
  Channel& operator=(const Channel&) = delete;

 private:
  twist::stdlike::atomic<uint32_t> has_result_{0};
  twist::stdlike::mutex mutex_;
  std::variant<std::monostate, T, std::exception_ptr> result_;
};

}  // namespace stdlike::detail

