/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

/* std::condition_variable provides the native_handle() method to return its
 * underlying pthread_cond_t*. WPILib uses this to interface with the FRC
 * network communication library. Since WPILib uses a custom mutex class and
 * not std::mutex, std::condition_variable_any must be used instead.
 * std::condition_variable_any doesn't expose its internal handle, so this
 * class provides the same interface and implementation in addition to a
 * native_handle() method.
 */

#include <condition_variable>
#include <memory>
#include <utility>

#include "priority_mutex.h"

namespace hal {

class priority_condition_variable {
  typedef std::chrono::system_clock clock;

 public:
  typedef std::condition_variable::native_handle_type native_handle_type;

  priority_condition_variable() : m_mutex(std::make_shared<std::mutex>()) {}
  ~priority_condition_variable() = default;

  priority_condition_variable(const priority_condition_variable&) = delete;
  priority_condition_variable& operator=(const priority_condition_variable&) =
      delete;

  void notify_one() noexcept {
    std::lock_guard<std::mutex> lock(*m_mutex);
    m_cond.notify_one();
  }

  void notify_all() noexcept {
    std::lock_guard<std::mutex> lock(*m_mutex);
    m_cond.notify_all();
  }

  template <typename Lock>
  void wait(Lock& _lock) {
    std::shared_ptr<std::mutex> _mutex = m_mutex;
    std::unique_lock<std::mutex> my_lock(*_mutex);
    Unlock<Lock> unlock(_lock);

    // *mutex must be unlocked before re-locking _lock so move
    // ownership of *_mutex lock to an object with shorter lifetime.
    std::unique_lock<std::mutex> my_lock2(std::move(my_lock));
    m_cond.wait(my_lock2);
  }

  template <typename Lock, typename Predicate>
  void wait(Lock& lock, Predicate p) {
    while (!p()) {
      wait(lock);
    }
  }

  template <typename Lock, typename Clock, typename Duration>
  std::cv_status wait_until(
      Lock& _lock, const std::chrono::time_point<Clock, Duration>& atime) {
    std::shared_ptr<std::mutex> _mutex = m_mutex;
    std::unique_lock<std::mutex> my_lock(*_mutex);
    Unlock<Lock> unlock(_lock);

    // *_mutex must be unlocked before re-locking _lock so move
    // ownership of *_mutex lock to an object with shorter lifetime.
    std::unique_lock<std::mutex> my_lock2(std::move(my_lock));
    return m_cond.wait_until(my_lock2, atime);
  }

  template <typename Lock, typename Clock, typename Duration,
            typename Predicate>
  bool wait_until(Lock& lock,
                  const std::chrono::time_point<Clock, Duration>& atime,
                  Predicate p) {
    while (!p()) {
      if (wait_until(lock, atime) == std::cv_status::timeout) {
        return p();
      }
    }
    return true;
  }

  template <typename Lock, typename Rep, typename Period>
  std::cv_status wait_for(Lock& lock,
                          const std::chrono::duration<Rep, Period>& rtime) {
    return wait_until(lock, clock::now() + rtime);
  }

  template <typename Lock, typename Rep, typename Period, typename Predicate>
  bool wait_for(Lock& lock, const std::chrono::duration<Rep, Period>& rtime,
                Predicate p) {
    return wait_until(lock, clock::now() + rtime, std::move(p));
  }

  native_handle_type native_handle() { return m_cond.native_handle(); }

 private:
  std::condition_variable m_cond;
  std::shared_ptr<std::mutex> m_mutex;

  // scoped unlock - unlocks in ctor, re-locks in dtor
  template <typename Lock>
  struct Unlock {
    explicit Unlock(Lock& lk) : m_lock(lk) { lk.unlock(); }

    ~Unlock() /*noexcept(false)*/ {
      if (std::uncaught_exception()) {
        try {
          m_lock.lock();
        } catch (...) {
        }
      } else {
        m_lock.lock();
      }
    }

    Unlock(const Unlock&) = delete;
    Unlock& operator=(const Unlock&) = delete;

    Lock& m_lock;
  };
};

}  // namespace hal

// For backwards compatibility
#ifndef NAMESPACED_PRIORITY
using hal::priority_condition_variable;  // NOLINT
#endif
