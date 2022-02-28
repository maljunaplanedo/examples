#pragma once

#include <memory>
#include <cassert>

#include <futures/channel.hpp>

namespace stdlike {

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

 public:
  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Movable
  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  // One-shot
  // Wait for result (value or exception)
  T Get() {
    return std::move(channel_->Get());
  }

 private:
  explicit Future(std::shared_ptr<detail::Channel<T>> channel)
      : channel_(channel) {
  }

 private:
  std::shared_ptr<detail::Channel<T>> channel_;
};

}  // namespace stdlike

