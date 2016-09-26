/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_ATOMIC_STATIC_H_
#define WPIUTIL_SUPPORT_ATOMIC_STATIC_H_

#if !defined(_MSC_VER) || (_MSC_VER >= 1900)

// Just use a local static.  This is thread-safe per
// http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/

// Per https://msdn.microsoft.com/en-us/library/Hh567368.aspx "Magic Statics"
// are supported in Visual Studio 2015 but not in earlier versions.
#define ATOMIC_STATIC(cls, inst) static cls inst
#define ATOMIC_STATIC_DECL(cls)
#define ATOMIC_STATIC_INIT(cls)

#else
// From http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/
#include <atomic>
#include <mutex>

#define ATOMIC_STATIC(cls, inst) \
    cls* inst##tmp = m_instance.load(std::memory_order_acquire); \
    if (inst##tmp == nullptr) { \
      std::lock_guard<std::mutex> lock(m_instance_mutex); \
      inst##tmp = m_instance.load(std::memory_order_relaxed); \
      if (inst##tmp == nullptr) { \
        inst##tmp = new cls; \
        m_instance.store(inst##tmp, std::memory_order_release); \
      } \
    } \
    cls& inst = *inst##tmp

#define ATOMIC_STATIC_DECL(cls) \
    static std::atomic<cls*> m_instance; \
    static std::mutex m_instance_mutex;

#define ATOMIC_STATIC_INIT(cls) \
    std::atomic<cls*> cls::m_instance; \
    std::mutex cls::m_instance_mutex;

#endif

#endif  // WPIUTIL_SUPPORT_ATOMIC_STATIC_H_
