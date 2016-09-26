//
// Copyright (c) 2013 Juan Palacios juan.palacios.puyana@gmail.com
// Subject to the BSD 2-Clause License
// - see < http://opensource.org/licenses/BSD-2-Clause>
//

#ifndef WPIUTIL_SUPPORT_CONCURRENT_QUEUE_H_
#define WPIUTIL_SUPPORT_CONCURRENT_QUEUE_H_

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace wpi {

template <typename T>
class ConcurrentQueue {
 public:
  bool empty() const {
    std::unique_lock<std::mutex> mlock(mutex_);
    return queue_.empty();
  }

  typename std::queue<T>::size_type size() const {
    std::unique_lock<std::mutex> mlock(mutex_);
    return queue_.size();
  }

  T pop() {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
      cond_.wait(mlock);
    }
    auto item = std::move(queue_.front());
    queue_.pop();
    return item;
  }

  void pop(T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
      cond_.wait(mlock);
    }
    item = queue_.front();
    queue_.pop();
  }

  void push(const T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }

  void push(T&& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(std::forward<T>(item));
    mlock.unlock();
    cond_.notify_one();
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.emplace(std::forward<Args>(args)...);
    mlock.unlock();
    cond_.notify_one();
  }

  ConcurrentQueue() = default;
  ConcurrentQueue(const ConcurrentQueue&) = delete;
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

 private:
  std::queue<T> queue_;
  mutable std::mutex mutex_;
  std::condition_variable cond_;
};

}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_CONCURRENT_QUEUE_H_
