#pragma once

#include <twist/stdlike/mutex.hpp>
#include <twist/stdlike/condition_variable.hpp>

#include <optional>
#include <deque>

namespace tp {

// Unbounded blocking multi-producers/multi-consumers queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  bool Put(T value) {
    std::lock_guard guard(mutex_);

    if (closed_) {
      return false;
    } else {
      buffer_.push_back(std::move(value));
      if (waiting_ > 0) {
        can_take_.notify_one();
      }
      return true;
    }
  }

  std::optional<T> Take() {
    std::unique_lock guard(mutex_);

    while (!closed_ && buffer_.empty()) {
      ++waiting_;
      can_take_.wait(guard);
      --waiting_;
    }

    if (buffer_.empty()) {
      return std::nullopt;
    }

    T value = std::move(buffer_.front());
    buffer_.pop_front();
    return value;
  }

  void Close() {
    CloseImpl(/*clear=*/false);
  }

  void Cancel() {
    CloseImpl(/*clear=*/true);
  }

 private:
  void CloseImpl(bool clear) {
    std::lock_guard guard(mutex_);
    closed_ = true;
    if (clear) {
      buffer_.clear();
    }
    can_take_.notify_all();
  }

 private:
  bool closed_{false};    //  guarded by mutex_
  std::deque<T> buffer_;  //  guarded by mutex_
  size_t waiting_{0};     //  guarded by mutex_
  twist::stdlike::mutex mutex_;
  twist::stdlike::condition_variable can_take_;
};

}  // namespace tp

