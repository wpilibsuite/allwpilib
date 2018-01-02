/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifdef __linux__
#include <pthread.h>
#endif

#include <condition_variable>
#include <memory>
#include <utility>

#include "priority_mutex.h"

namespace wpi {

#if defined(__linux__) && defined(WPI_HAVE_PRIORITY_MUTEX)

#define WPI_HAVE_PRIORITY_CONDITION_VARIABLE 1

class priority_condition_variable {
  typedef std::chrono::system_clock clock;

 public:
  typedef pthread_cond_t* native_handle_type;

  priority_condition_variable() noexcept = default;
  ~priority_condition_variable() noexcept { pthread_cond_destroy(&m_cond); }

  priority_condition_variable(const priority_condition_variable&) = delete;
  priority_condition_variable& operator=(const priority_condition_variable&) =
      delete;

  void notify_one() noexcept { pthread_cond_signal(&m_cond); }

  void notify_all() noexcept { pthread_cond_broadcast(&m_cond); }

  void wait(std::unique_lock<priority_mutex>& lock) noexcept {
    int e = pthread_cond_wait(&m_cond, lock.mutex()->native_handle());
    if (e) std::terminate();
  }

  template <typename Predicate>
  void wait(std::unique_lock<priority_mutex>& lock, Predicate p) {
    while (!p()) {
      wait(lock);
    }
  }

  template <typename Duration>
  std::cv_status wait_until(
      std::unique_lock<priority_mutex>& lock,
      const std::chrono::time_point<clock, Duration>& atime) {
    return wait_until_impl(lock, atime);
  }

  template <typename Clock, typename Duration>
  std::cv_status wait_until(
      std::unique_lock<priority_mutex>& lock,
      const std::chrono::time_point<Clock, Duration>& atime) {
    const typename Clock::time_point c_entry = Clock::now();
    const clock::time_point s_entry = clock::now();
    const auto delta = atime - c_entry;
    const auto s_atime = s_entry + delta;

    return wait_until_impl(lock, s_atime);
  }

  template <typename Clock, typename Duration, typename Predicate>
  bool wait_until(std::unique_lock<priority_mutex>& lock,
                  const std::chrono::time_point<Clock, Duration>& atime,
                  Predicate p) {
    while (!p()) {
      if (wait_until(lock, atime) == std::cv_status::timeout) {
        return p();
      }
    }
    return true;
  }

  template <typename Rep, typename Period>
  std::cv_status wait_for(std::unique_lock<priority_mutex>& lock,
                          const std::chrono::duration<Rep, Period>& rtime) {
    return wait_until(lock, clock::now() + rtime);
  }

  template <typename Rep, typename Period, typename Predicate>
  bool wait_for(std::unique_lock<priority_mutex>& lock,
                const std::chrono::duration<Rep, Period>& rtime, Predicate p) {
    return wait_until(lock, clock::now() + rtime, std::move(p));
  }

  native_handle_type native_handle() { return &m_cond; }

 private:
  pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;

  template <typename Dur>
  std::cv_status wait_until_impl(
      std::unique_lock<priority_mutex>& lock,
      const std::chrono::time_point<clock, Dur>& atime) {
    auto s = std::chrono::time_point_cast<std::chrono::seconds>(atime);
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(atime - s);

    struct timespec ts = {
        static_cast<std::time_t>(s.time_since_epoch().count()),
        static_cast<long>(ns.count())};  // NOLINT(runtime/int)

    pthread_cond_timedwait(&m_cond, lock.mutex()->native_handle(), &ts);

    return (clock::now() < atime ? std::cv_status::no_timeout
                                 : std::cv_status::timeout);
  }
};
#endif

}  // namespace wpi
