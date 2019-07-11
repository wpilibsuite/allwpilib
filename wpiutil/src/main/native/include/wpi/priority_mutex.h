/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

// Allows usage with std::scoped_lock without including <mutex> separately
#ifdef __linux__
#include <pthread.h>
#endif

#include <mutex>

namespace wpi {

#if defined(__FRC_ROBORIO__) && !defined(WPI_USE_PRIORITY_MUTEX)
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
#ifdef __PTHREAD_MUTEX_HAVE_PREV
  pthread_mutex_t m_mutex = {
      {0, 0, 0, 0, 0x20 | PTHREAD_MUTEX_RECURSIVE_NP, __PTHREAD_SPINS, {0, 0}}};
#else
  pthread_mutex_t m_mutex = {
      {0, 0, 0, 0x20 | PTHREAD_MUTEX_RECURSIVE_NP, 0, {__PTHREAD_SPINS}}};
#endif
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
#ifdef __PTHREAD_MUTEX_HAVE_PREV
  pthread_mutex_t m_mutex = {{0, 0, 0, 0, 0x20, __PTHREAD_SPINS, {0, 0}}};
#else
  pthread_mutex_t m_mutex = {{0, 0, 0, 0x20, 0, {__PTHREAD_SPINS}}};
#endif
};

#endif  // defined(WPI_USE_PRIORITY_MUTEX) && defined(__linux__)

}  // namespace wpi
