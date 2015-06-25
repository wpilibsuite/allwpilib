#pragma once

#include <pthread.h>

// Allows usage with std::unique_lock without including <mutex> separately
#include <mutex>

class priority_recursive_mutex {
 public:
  typedef pthread_mutex_t *native_handle_type;

  constexpr priority_recursive_mutex() noexcept = default;
  priority_recursive_mutex(const priority_recursive_mutex &) = delete;
  priority_recursive_mutex &operator=(const priority_recursive_mutex &) = delete;

  // Lock the mutex, blocking until it's available.
  void lock();

  // Unlock the mutex.
  void unlock();

  // Tries to lock the mutex.
  bool try_lock() noexcept;

  pthread_mutex_t *native_handle();

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
  typedef pthread_mutex_t *native_handle_type;

  constexpr priority_mutex() noexcept = default;
  priority_mutex(const priority_mutex &) = delete;
  priority_mutex &operator=(const priority_mutex &) = delete;

  // Lock the mutex, blocking until it's available.
  void lock();

  // Unlock the mutex.
  void unlock();

  // Tries to lock the mutex.
  bool try_lock() noexcept;

  pthread_mutex_t *native_handle();

 private:
  // Do the equivalent of setting PTHREAD_PRIO_INHERIT.
#if __WORDSIZE == 64
  pthread_mutex_t m_mutex = {
      {0, 0, 0, 0, 0x20, 0, 0, {0, 0}}};
#else
  pthread_mutex_t m_mutex = {
      {0, 0, 0, 0x20, 0, {0}}};
#endif
};
