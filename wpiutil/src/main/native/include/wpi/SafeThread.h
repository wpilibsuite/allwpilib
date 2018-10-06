/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_SAFETHREAD_H_
#define WPIUTIL_WPI_SAFETHREAD_H_

#include <atomic>
#include <memory>
#include <thread>
#include <utility>

#include "wpi/condition_variable.h"
#include "wpi/mutex.h"

namespace wpi {

// Base class for SafeThreadOwner threads.
class SafeThread {
 public:
  virtual ~SafeThread() = default;
  virtual void Main() = 0;

  mutable wpi::mutex m_mutex;
  std::atomic_bool m_active{true};
  wpi::condition_variable m_cond;
};

namespace detail {

// Non-template proxy base class for common proxy code.
class SafeThreadProxyBase {
 public:
  explicit SafeThreadProxyBase(std::shared_ptr<SafeThread> thr);
  explicit operator bool() const { return m_thread != nullptr; }
  std::unique_lock<wpi::mutex>& GetLock() { return m_lock; }

 protected:
  std::shared_ptr<SafeThread> m_thread;
  std::unique_lock<wpi::mutex> m_lock;
};

// A proxy for SafeThread.
// Also serves as a scoped lock on SafeThread::m_mutex.
template <typename T>
class SafeThreadProxy : public SafeThreadProxyBase {
 public:
  explicit SafeThreadProxy(std::shared_ptr<SafeThread> thr)
      : SafeThreadProxyBase(std::move(thr)) {}
  T& operator*() const { return *static_cast<T*>(m_thread.get()); }
  T* operator->() const { return static_cast<T*>(m_thread.get()); }
};

// Non-template owner base class for common owner code.
class SafeThreadOwnerBase {
 public:
  void Stop();
  void Join();

  SafeThreadOwnerBase() noexcept = default;
  SafeThreadOwnerBase(const SafeThreadOwnerBase&) = delete;
  SafeThreadOwnerBase& operator=(const SafeThreadOwnerBase&) = delete;
  SafeThreadOwnerBase(SafeThreadOwnerBase&& other) noexcept
      : SafeThreadOwnerBase() {
    swap(*this, other);
  }
  SafeThreadOwnerBase& operator=(SafeThreadOwnerBase other) noexcept {
    swap(*this, other);
    return *this;
  }
  ~SafeThreadOwnerBase();

  friend void swap(SafeThreadOwnerBase& lhs, SafeThreadOwnerBase& rhs) noexcept;

  explicit operator bool() const;

  std::thread::native_handle_type GetNativeThreadHandle();

  void SetJoinAtExit(bool joinAtExit) { m_joinAtExit = joinAtExit; }

 protected:
  void Start(std::shared_ptr<SafeThread> thr);
  std::shared_ptr<SafeThread> GetThread() const;

 private:
  mutable wpi::mutex m_mutex;
  std::thread m_stdThread;
  std::weak_ptr<SafeThread> m_thread;
  std::atomic_bool m_joinAtExit{true};
};

void swap(SafeThreadOwnerBase& lhs, SafeThreadOwnerBase& rhs) noexcept;

}  // namespace detail

template <typename T>
class SafeThreadOwner : public detail::SafeThreadOwnerBase {
 public:
  template <typename... Args>
  void Start(Args&&... args) {
    detail::SafeThreadOwnerBase::Start(
        std::make_shared<T>(std::forward<Args>(args)...));
  }

  using Proxy = typename detail::SafeThreadProxy<T>;
  Proxy GetThread() const {
    return Proxy(detail::SafeThreadOwnerBase::GetThread());
  }
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_SAFETHREAD_H_
