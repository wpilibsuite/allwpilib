// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <cassert>
#include <mutex>
#include <thread>

#include "Compiler.h"

namespace wpi {

/**
 * A spinlock mutex.  Wraps std::atomic_flag in a std::mutex compatible way.
 */
class spinlock {
  std::atomic_flag lock_flag;

 public:
  spinlock() noexcept { lock_flag.clear(); }

  LLVM_ATTRIBUTE_ALWAYS_INLINE
  bool try_lock() { return !lock_flag.test_and_set(std::memory_order_acquire); }

  LLVM_ATTRIBUTE_ALWAYS_INLINE
  void lock() {
    for (unsigned int i = 1; !try_lock(); ++i) {
      if ((i & 0xff) == 0) {
        std::this_thread::yield();
      }
    }
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE
  void unlock() { lock_flag.clear(std::memory_order_release); }
};

/**
 * A recursive spinlock mutex.  This version uses std::atomic_flag for spin,
 * then checks the thread id for recursion.  It is generally faster on desktop
 * platforms compared to recursive_spinlock2.
 */
class recursive_spinlock1 {
  std::atomic<std::thread::id> owner_thread_id{std::thread::id{}};
  int32_t recursive_counter{0};
  std::atomic_flag lock_flag;

 public:
  recursive_spinlock1() noexcept { lock_flag.clear(); }

  LLVM_ATTRIBUTE_ALWAYS_INLINE
  bool try_lock() {
    if (!lock_flag.test_and_set(std::memory_order_acquire)) {
      owner_thread_id.store(std::this_thread::get_id(),
                            std::memory_order_release);
    } else {
      if (owner_thread_id.load(std::memory_order_acquire) !=
          std::this_thread::get_id()) {
        return false;
      }
    }
    ++recursive_counter;
    return true;
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE
  void lock() {
    for (unsigned int i = 1; !try_lock(); ++i) {
      if ((i & 0xffff) == 0) {
        std::this_thread::yield();
      }
    }
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE
  void unlock() {
    assert(owner_thread_id.load(std::memory_order_acquire) ==
           std::this_thread::get_id());
    assert(recursive_counter > 0);

    if (--recursive_counter == 0) {
      owner_thread_id.store(std::thread::id{}, std::memory_order_release);
      lock_flag.clear(std::memory_order_release);
    }
  }
};

/**
 * A recursive spinlock mutex.  This version spins directly on the std::atomic
 * of the thread id.  It is generally faster on embedded ARM platforms such
 * as the RoboRIO and Raspberry Pi, compared to recursive_spinlock1.
 */
class recursive_spinlock2 {
  std::atomic<std::thread::id> owner_thread_id{std::thread::id{}};
  int32_t recursive_counter{0};

 public:
  LLVM_ATTRIBUTE_ALWAYS_INLINE
  bool try_lock() {
    auto owner = std::thread::id{};
    auto us = std::this_thread::get_id();
    if (!owner_thread_id.compare_exchange_weak(owner, us,
                                               std::memory_order_acquire)) {
      if (owner != us) {
        return false;
      }
    }
    ++recursive_counter;
    return true;
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE
  void lock() {
    for (unsigned int i = 1; !try_lock(); ++i) {
      if ((i & 0xffff) == 0) {
        std::this_thread::yield();
      }
    }
  }

  LLVM_ATTRIBUTE_ALWAYS_INLINE
  void unlock() {
    assert(owner_thread_id.load(std::memory_order_acquire) ==
           std::this_thread::get_id());
    assert(recursive_counter > 0);

    if (--recursive_counter == 0) {
      owner_thread_id.store(std::thread::id{}, std::memory_order_release);
    }
  }
};

#ifdef __arm__
// benchmarking has shown this version to be faster on ARM, but slower on
// windows, mac, and linux
using recursive_spinlock = recursive_spinlock2;
#else
using recursive_spinlock = recursive_spinlock1;
#endif

}  // namespace wpi
