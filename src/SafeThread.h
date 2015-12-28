/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_SAFETHREAD_H_
#define NT_SAFETHREAD_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace nt {

// Base class for SafeThreadOwner threads.
class SafeThread {
 public:
  virtual ~SafeThread() = default;
  virtual void Main() = 0;

  std::mutex m_mutex;
  bool m_active = true;
  std::condition_variable m_cond;
};

namespace detail {

// Non-template proxy base class for common proxy code.
class SafeThreadProxyBase {
 public:
  SafeThreadProxyBase(SafeThread* thr) : m_thread(thr) {
    if (!m_thread) return;
    std::unique_lock<std::mutex>(m_thread->m_mutex).swap(m_lock);
    if (!m_thread->m_active) {
      m_lock.unlock();
      m_thread = nullptr;
      return;
    }
  }
  explicit operator bool() const { return m_thread != nullptr; }
  std::unique_lock<std::mutex>& GetLock() { return m_lock; }

 protected:
  SafeThread* m_thread;
  std::unique_lock<std::mutex> m_lock;
};

// A proxy for SafeThread.
// Also serves as a scoped lock on SafeThread::m_mutex.
template <typename T>
class SafeThreadProxy : public SafeThreadProxyBase {
 public:
  SafeThreadProxy(SafeThread* thr) : SafeThreadProxyBase(thr) {}
  T& operator*() const { return *static_cast<T*>(m_thread); }
  T* operator->() const { return static_cast<T*>(m_thread); }
};

// Non-template owner base class for common owner code.
class SafeThreadOwnerBase {
 public:
  void Stop();

 protected:
  SafeThreadOwnerBase() { m_thread = nullptr; }
  SafeThreadOwnerBase(const SafeThreadOwnerBase&) = delete;
  SafeThreadOwnerBase& operator=(const SafeThreadOwnerBase&) = delete;
  ~SafeThreadOwnerBase() { Stop(); }

  void Start(SafeThread* thr);
  SafeThread* GetThread() { return m_thread.load(); }

 private:
  std::atomic<SafeThread*> m_thread;
};

}  // namespace detail

template <typename T>
class SafeThreadOwner : public detail::SafeThreadOwnerBase {
 public:
  void Start() { Start(new T); }
  void Start(T* thr) { detail::SafeThreadOwnerBase::Start(thr); }

  using Proxy = typename detail::SafeThreadProxy<T>;
  Proxy GetThread() { return Proxy(detail::SafeThreadOwnerBase::GetThread()); }
};

}  // namespace nt

#endif  // NT_SAFETHREAD_H_
