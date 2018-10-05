/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_CALLBACKMANAGER_H_
#define NTCORE_CALLBACKMANAGER_H_

#include <atomic>
#include <climits>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include <wpi/SafeThread.h>
#include <wpi/UidVector.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

namespace nt {

namespace impl {

template <typename Callback>
class ListenerData {
 public:
  ListenerData() = default;
  explicit ListenerData(Callback callback_) : callback(callback_) {}
  explicit ListenerData(unsigned int poller_uid_) : poller_uid(poller_uid_) {}

  explicit operator bool() const { return callback || poller_uid != UINT_MAX; }

  Callback callback;
  unsigned int poller_uid = UINT_MAX;
};

// CRTP callback manager thread
// @tparam Derived        derived class
// @tparam NotifierData   data buffered for each callback
// @tparam ListenerData   data stored for each listener
// Derived must define the following functions:
//   bool Matches(const ListenerData& listener, const NotifierData& data);
//   void SetListener(NotifierData* data, unsigned int listener_uid);
//   void DoCallback(Callback callback, const NotifierData& data);
template <typename Derived, typename TUserInfo,
          typename TListenerData =
              ListenerData<std::function<void(const TUserInfo& info)>>,
          typename TNotifierData = TUserInfo>
class CallbackThread : public wpi::SafeThread {
 public:
  typedef TUserInfo UserInfo;
  typedef TNotifierData NotifierData;
  typedef TListenerData ListenerData;

  ~CallbackThread() {
    // Wake up any blocked pollers
    for (size_t i = 0; i < m_pollers.size(); ++i) {
      if (auto poller = m_pollers[i]) poller->Terminate();
    }
  }

  void Main() override;

  wpi::UidVector<ListenerData, 64> m_listeners;

  std::queue<std::pair<unsigned int, NotifierData>> m_queue;
  wpi::condition_variable m_queue_empty;

  struct Poller {
    void Terminate() {
      {
        std::lock_guard<wpi::mutex> lock(poll_mutex);
        terminating = true;
      }
      poll_cond.notify_all();
    }
    std::queue<NotifierData> poll_queue;
    wpi::mutex poll_mutex;
    wpi::condition_variable poll_cond;
    bool terminating = false;
    bool cancelling = false;
  };
  wpi::UidVector<std::shared_ptr<Poller>, 64> m_pollers;

  // Must be called with m_mutex held
  template <typename... Args>
  void SendPoller(unsigned int poller_uid, Args&&... args) {
    if (poller_uid > m_pollers.size()) return;
    auto poller = m_pollers[poller_uid];
    if (!poller) return;
    {
      std::lock_guard<wpi::mutex> lock(poller->poll_mutex);
      poller->poll_queue.emplace(std::forward<Args>(args)...);
    }
    poller->poll_cond.notify_one();
  }
};

template <typename Derived, typename TUserInfo, typename TListenerData,
          typename TNotifierData>
void CallbackThread<Derived, TUserInfo, TListenerData, TNotifierData>::Main() {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  while (m_active) {
    while (m_queue.empty()) {
      m_cond.wait(lock);
      if (!m_active) return;
    }

    while (!m_queue.empty()) {
      if (!m_active) return;
      auto item = std::move(m_queue.front());

      if (item.first != UINT_MAX) {
        if (item.first < m_listeners.size()) {
          auto& listener = m_listeners[item.first];
          if (listener &&
              static_cast<Derived*>(this)->Matches(listener, item.second)) {
            static_cast<Derived*>(this)->SetListener(&item.second, item.first);
            if (listener.callback) {
              lock.unlock();
              static_cast<Derived*>(this)->DoCallback(listener.callback,
                                                      item.second);
              lock.lock();
            } else if (listener.poller_uid != UINT_MAX) {
              SendPoller(listener.poller_uid, std::move(item.second));
            }
          }
        }
      } else {
        // Use index because iterator might get invalidated.
        for (size_t i = 0; i < m_listeners.size(); ++i) {
          auto& listener = m_listeners[i];
          if (!listener) continue;
          if (!static_cast<Derived*>(this)->Matches(listener, item.second))
            continue;
          static_cast<Derived*>(this)->SetListener(&item.second, i);
          if (listener.callback) {
            lock.unlock();
            static_cast<Derived*>(this)->DoCallback(listener.callback,
                                                    item.second);
            lock.lock();
          } else if (listener.poller_uid != UINT_MAX) {
            SendPoller(listener.poller_uid, item.second);
          }
        }
      }
      m_queue.pop();
    }

    m_queue_empty.notify_all();
  }
}

}  // namespace impl

// CRTP callback manager
// @tparam Derived  derived class
// @tparam Thread   custom thread (must be derived from impl::CallbackThread)
//
// Derived must define the following functions:
//   void Start();
template <typename Derived, typename Thread>
class CallbackManager {
  friend class RpcServerTest;

 public:
  void Stop() { m_owner.Stop(); }

  void Remove(unsigned int listener_uid) {
    auto thr = m_owner.GetThread();
    if (!thr) return;
    thr->m_listeners.erase(listener_uid);
  }

  unsigned int CreatePoller() {
    static_cast<Derived*>(this)->Start();
    auto thr = m_owner.GetThread();
    return thr->m_pollers.emplace_back(
        std::make_shared<typename Thread::Poller>());
  }

  void RemovePoller(unsigned int poller_uid) {
    auto thr = m_owner.GetThread();
    if (!thr) return;

    // Remove any listeners that are associated with this poller
    for (size_t i = 0; i < thr->m_listeners.size(); ++i) {
      if (thr->m_listeners[i].poller_uid == poller_uid)
        thr->m_listeners.erase(i);
    }

    // Wake up any blocked pollers
    if (poller_uid >= thr->m_pollers.size()) return;
    auto poller = thr->m_pollers[poller_uid];
    if (!poller) return;
    poller->Terminate();
    return thr->m_pollers.erase(poller_uid);
  }

  bool WaitForQueue(double timeout) {
    auto thr = m_owner.GetThread();
    if (!thr) return true;

    auto& lock = thr.GetLock();
    auto timeout_time = std::chrono::steady_clock::now() +
                        std::chrono::duration<double>(timeout);
    while (!thr->m_queue.empty()) {
      if (!thr->m_active) return true;
      if (timeout == 0) return false;
      if (timeout < 0) {
        thr->m_queue_empty.wait(lock);
      } else {
        auto cond_timed_out = thr->m_queue_empty.wait_until(lock, timeout_time);
        if (cond_timed_out == std::cv_status::timeout) return false;
      }
    }

    return true;
  }

  std::vector<typename Thread::UserInfo> Poll(unsigned int poller_uid) {
    bool timed_out = false;
    return Poll(poller_uid, -1, &timed_out);
  }

  std::vector<typename Thread::UserInfo> Poll(unsigned int poller_uid,
                                              double timeout, bool* timed_out) {
    std::vector<typename Thread::UserInfo> infos;
    std::shared_ptr<typename Thread::Poller> poller;
    {
      auto thr = m_owner.GetThread();
      if (!thr) return infos;
      if (poller_uid > thr->m_pollers.size()) return infos;
      poller = thr->m_pollers[poller_uid];
      if (!poller) return infos;
    }

    std::unique_lock<wpi::mutex> lock(poller->poll_mutex);
    auto timeout_time = std::chrono::steady_clock::now() +
                        std::chrono::duration<double>(timeout);
    *timed_out = false;
    while (poller->poll_queue.empty()) {
      if (poller->terminating) return infos;
      if (poller->cancelling) {
        // Note: this only works if there's a single thread calling this
        // function for any particular poller, but that's the intended use.
        poller->cancelling = false;
        return infos;
      }
      if (timeout == 0) {
        *timed_out = true;
        return infos;
      }
      if (timeout < 0) {
        poller->poll_cond.wait(lock);
      } else {
        auto cond_timed_out = poller->poll_cond.wait_until(lock, timeout_time);
        if (cond_timed_out == std::cv_status::timeout) {
          *timed_out = true;
          return infos;
        }
      }
    }

    while (!poller->poll_queue.empty()) {
      infos.emplace_back(std::move(poller->poll_queue.front()));
      poller->poll_queue.pop();
    }
    return infos;
  }

  void CancelPoll(unsigned int poller_uid) {
    std::shared_ptr<typename Thread::Poller> poller;
    {
      auto thr = m_owner.GetThread();
      if (!thr) return;
      if (poller_uid > thr->m_pollers.size()) return;
      poller = thr->m_pollers[poller_uid];
      if (!poller) return;
    }

    {
      std::lock_guard<wpi::mutex> lock(poller->poll_mutex);
      poller->cancelling = true;
    }
    poller->poll_cond.notify_one();
  }

 protected:
  template <typename... Args>
  void DoStart(Args&&... args) {
    m_owner.Start(std::forward<Args>(args)...);
  }

  template <typename... Args>
  unsigned int DoAdd(Args&&... args) {
    static_cast<Derived*>(this)->Start();
    auto thr = m_owner.GetThread();
    return thr->m_listeners.emplace_back(std::forward<Args>(args)...);
  }

  template <typename... Args>
  void Send(unsigned int only_listener, Args&&... args) {
    auto thr = m_owner.GetThread();
    if (!thr || thr->m_listeners.empty()) return;
    thr->m_queue.emplace(std::piecewise_construct,
                         std::make_tuple(only_listener),
                         std::forward_as_tuple(std::forward<Args>(args)...));
    thr->m_cond.notify_one();
  }

  typename wpi::SafeThreadOwner<Thread>::Proxy GetThread() const {
    return m_owner.GetThread();
  }

 private:
  wpi::SafeThreadOwner<Thread> m_owner;
};

}  // namespace nt

#endif  // NTCORE_CALLBACKMANAGER_H_
