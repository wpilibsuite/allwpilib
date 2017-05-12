/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

// Allows usage with std::lock_guard without including <mutex> separately
#include <mutex>

#if defined(FRC_SIMULATOR) || defined(_WIN32)
namespace hal {
// We do not want to use pthreads if in the simulator; however, in the
// simulator, we do not care about priority inversion.
typedef std::mutex priority_mutex;
typedef std::recursive_mutex priority_recursive_mutex;
}  // namespace hal
#else  // Covers rest of file.

#include <pthread.h>

namespace hal {

class priority_recursive_mutex {
 public:
  typedef pthread_mutex_t* native_handle_type;

  constexpr priority_recursive_mutex() noexcept = default;
  priority_recursive_mutex(const priority_recursive_mutex&) = delete;
  priority_recursive_mutex& operator=(const priority_recursive_mutex&) = delete;

  // Lock the mutex, blocking until it's available.
  void lock();

  // Unlock the mutex.
  void unlock();

  // Tries to lock the mutex.
  bool try_lock() noexcept;

  pthread_mutex_t* native_handle();

 private:
// Do the equivalent of setting PTHREAD_PRIO_INHERIT and
// PTHREAD_MUTEX_RECURSIVE_NP.
#if __WORDSIZE == 64
  pthread_mutex_t m_mutex = {
      {0, 0, 0, 0, 0x20 | PTHREAD_MUTEX_RECURSIVE_NP, 0, 0, {0, 0}}};
#else
  pthread_mutex_t m_mutex = {
      {0, 0, 0, 0x20 | PTHREAD_MUTEX_RECURSIVE_NP, 0, {0}}};
#endif
};

class priority_mutex {
 public:
  typedef pthread_mutex_t* native_handle_type;

  constexpr priority_mutex() noexcept = default;
  priority_mutex(const priority_mutex&) = delete;
  priority_mutex& operator=(const priority_mutex&) = delete;

  // Lock the mutex, blocking until it's available.
  void lock();

  // Unlock the mutex.
  void unlock();

  // Tries to lock the mutex.
  bool try_lock() noexcept;

  pthread_mutex_t* native_handle();

 private:
// Do the equivalent of setting PTHREAD_PRIO_INHERIT.
#if __WORDSIZE == 64
  pthread_mutex_t m_mutex = {{0, 0, 0, 0, 0x20, 0, 0, {0, 0}}};
#else
  pthread_mutex_t m_mutex = {{0, 0, 0, 0x20, 0, {0}}};
#endif
};
}  // namespace hal

#endif  // FRC_SIMULATOR

// For backwards compatibility
#ifndef NAMESPACED_PRIORITY
using hal::priority_mutex;            // NOLINT
using hal::priority_recursive_mutex;  // NOLINT
#endif
