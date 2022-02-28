#pragma once

#include "tagged_semaphore.hpp"

#include <deque>

namespace solutions {

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BlockingQueue {
 private:
  class SpacePermitTag {};
  class MutexLikePermitTag {};

 public:
  explicit BlockingQueue(size_t capacity)
      : taken_space_(0), available_space_(capacity), mutex_like_(1) {
  }

  // Inserts the specified element into this queue,
  // waiting if necessary for space to become available.
  void Put(T value) {
    auto space_token = available_space_.Acquire();
    auto guard = mutex_like_.MakeGuard();

    buffer_.push_front(std::move(value));

    taken_space_.Release(std::move(space_token));
  }

  // Retrieves and removes the head of this queue,
  // waiting if necessary until an element becomes available
  T Take() {
    auto space_token = taken_space_.Acquire();
    auto guard = mutex_like_.MakeGuard();

    T value = std::move(buffer_.back());
    buffer_.pop_back();

    available_space_.Release(std::move(space_token));

    return value;
  }

 private:
  std::deque<T> buffer_;

  TaggedSemaphore<SpacePermitTag> taken_space_;
  TaggedSemaphore<SpacePermitTag> available_space_;
  TaggedSemaphore<MutexLikePermitTag> mutex_like_;
};

}  // namespace solutions

