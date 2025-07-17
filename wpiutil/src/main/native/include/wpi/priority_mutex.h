// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

// Allows usage with std::scoped_lock without including <mutex> separately
#ifdef __linux__
#include <pthread.h>
#endif

#include <mutex>

namespace wpi {

#if defined(__FRC_SYSTEMCORE__) && !defined(WPI_USE_PRIORITY_MUTEX)
#define WPI_USE_PRIORITY_MUTEX
#endif

#if defined(WPI_USE_PRIORITY_MUTEX) && defined(__linux__)

#define WPI_HAVE_PRIORITY_MUTEX 1

class priority_recursive_mutex {
 public:
  using native_handle_type = pthread_mutex_t*;

  constexpr priority_recursive_mutex() noexcept = default;
  priority_recursive_mutex(const priority_recursive_mutex&) = delete;
  priority_recursive_mutex& operator=(const priority_recursive_mutex&) = delete;

  // Lock the mutex, blocking until it's available.
  void lock() { pthread_mutex_lock(&m_mutex); }

  // Unlock the mutex.
  void unlock() { pthread_mutex_unlock(&m_mutex); }

  // Tries to lock the mutex.
  bool try_lock() noexcept { return !pthread_mutex_trylock(&m_mutex); }

  pthread_mutex_t* native_handle() { return &m_mutex; }

 private:
  // Do the equivalent of setting PTHREAD_PRIO_INHERIT and
  // PTHREAD_MUTEX_RECURSIVE_NP.
  // from glibc sysdeps/nptl/pthreadP.h: PTHREAD_MUTEX_PRIO_INHERIT_NP = 32
  pthread_mutex_t m_mutex = {
      {__PTHREAD_MUTEX_INITIALIZER(0x20 | PTHREAD_MUTEX_RECURSIVE_NP)}};
};

class priority_mutex {
 public:
  using native_handle_type = pthread_mutex_t*;

  constexpr priority_mutex() noexcept = default;
  priority_mutex(const priority_mutex&) = delete;
  priority_mutex& operator=(const priority_mutex&) = delete;

  // Lock the mutex, blocking until it's available.
  void lock() { pthread_mutex_lock(&m_mutex); }

  // Unlock the mutex.
  void unlock() { pthread_mutex_unlock(&m_mutex); }

  // Tries to lock the mutex.
  bool try_lock() noexcept { return !pthread_mutex_trylock(&m_mutex); }

  pthread_mutex_t* native_handle() { return &m_mutex; }

 private:
  // Do the equivalent of setting PTHREAD_PRIO_INHERIT.
  // from glibc sysdeps/nptl/pthreadP.h: PTHREAD_MUTEX_PRIO_INHERIT_NP = 32
  pthread_mutex_t m_mutex = {{__PTHREAD_MUTEX_INITIALIZER(0x20)}};
};

#endif  // defined(WPI_USE_PRIORITY_MUTEX) && defined(__linux__)

}  // namespace wpi
