
#pragma once

#include <twist/stdlike/mutex.hpp>

namespace util {

//////////////////////////////////////////////////////////////////////

// Safe API for mutual exclusion

template <typename T>
class Mutexed {
  using MutexImpl = twist::stdlike::mutex;

  class UniqueRef {
   public:
    // Non-copyable
    UniqueRef(const UniqueRef&) = delete;

    // Non-movable
    UniqueRef(UniqueRef&&) = delete;

    UniqueRef(T& object, MutexImpl& mutex) : object_(object), guard_(mutex) {
    }

    T& operator*() {
      return object_;
    }

    T* operator->() {
      return &object_;
    }

   private:
    T& object_;
    std::lock_guard<MutexImpl> guard_;
  };

 public:
  // https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
  template <typename... Args>
  explicit Mutexed(Args&&... args) : object_(std::forward<Args>(args)...) {
  }

  UniqueRef Lock() {
    return {object_, mutex_};
  }

 private:
  T object_;
  MutexImpl mutex_;  // Guards access to object_
};

//////////////////////////////////////////////////////////////////////

// Helper function for single operations over shared object:
// Usage:
//   Mutexed<vector<int>> ints;
//   Locked(ints)->push_back(42);

template <typename T>
auto Locked(Mutexed<T>& object) {
  return object.Lock();
}

}  // namespace util

