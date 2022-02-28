#include <twist/stdlike/condition_variable.hpp>
#include <twist/stdlike/mutex.hpp>

namespace tp::detail {

class BlockingZeroWaitMPMCCounter {
 public:
  BlockingZeroWaitMPMCCounter() {
  }

  void Inc() {
    std::lock_guard guard(mutex_);
    ++counter_;
  }

  void Dec() {
    std::lock_guard guard(mutex_);
    --counter_;
    if (counter_ == 0) {
      is_zero_.notify_all();
    }
  }

  void Wait() {
    std::unique_lock guard(mutex_);
    while (counter_ > 0) {
      is_zero_.wait(guard);
    }
  }

 private:
  size_t counter_{0};  // guarded by mutex

  twist::stdlike::mutex mutex_;
  twist::stdlike::condition_variable is_zero_;
};

}  // namespace tp::detail

