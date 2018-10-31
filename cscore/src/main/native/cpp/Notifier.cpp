/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Notifier.h"

#include <queue>
#include <vector>

#include "Handle.h"
#include "Instance.h"
#include "SinkImpl.h"
#include "SourceImpl.h"

using namespace cs;

bool Notifier::s_destroyed = false;

namespace {
// Vector which provides an integrated freelist for removal and reuse of
// individual elements.
template <typename T>
class UidVector {
 public:
  using size_type = typename std::vector<T>::size_type;

  size_type size() const { return m_vector.size(); }
  T& operator[](size_type i) { return m_vector[i]; }
  const T& operator[](size_type i) const { return m_vector[i]; }

  // Add a new T to the vector.  If there are elements on the freelist,
  // reuses the last one; otherwise adds to the end of the vector.
  // Returns the resulting element index (+1).
  template <class... Args>
  unsigned int emplace_back(Args&&... args) {
    unsigned int uid;
    if (m_free.empty()) {
      uid = m_vector.size();
      m_vector.emplace_back(std::forward<Args>(args)...);
    } else {
      uid = m_free.back();
      m_free.pop_back();
      m_vector[uid] = T(std::forward<Args>(args)...);
    }
    return uid + 1;
  }

  // Removes the identified element by replacing it with a default-constructed
  // one.  The element is added to the freelist for later reuse.
  void erase(unsigned int uid) {
    --uid;
    if (uid >= m_vector.size() || !m_vector[uid]) return;
    m_free.push_back(uid);
    m_vector[uid] = T();
  }

 private:
  std::vector<T> m_vector;
  std::vector<unsigned int> m_free;
};

}  // namespace

class Notifier::Thread : public wpi::SafeThread {
 public:
  Thread(std::function<void()> on_start, std::function<void()> on_exit)
      : m_on_start(on_start), m_on_exit(on_exit) {}

  void Main();

  struct Listener {
    Listener() = default;
    Listener(std::function<void(const RawEvent& event)> callback_,
             int eventMask_)
        : callback(callback_), eventMask(eventMask_) {}

    explicit operator bool() const { return static_cast<bool>(callback); }

    std::string prefix;
    std::function<void(const RawEvent& event)> callback;
    int eventMask;
  };
  UidVector<Listener> m_listeners;

  std::queue<RawEvent> m_notifications;

  std::function<void()> m_on_start;
  std::function<void()> m_on_exit;
};

Notifier::Notifier() { s_destroyed = false; }

Notifier::~Notifier() { s_destroyed = true; }

void Notifier::Start() { m_owner.Start(m_on_start, m_on_exit); }

void Notifier::Stop() { m_owner.Stop(); }

void Notifier::Thread::Main() {
  if (m_on_start) m_on_start();

  std::unique_lock<wpi::mutex> lock(m_mutex);
  while (m_active) {
    while (m_notifications.empty()) {
      m_cond.wait(lock);
      if (!m_active) goto done;
    }

    while (!m_notifications.empty()) {
      if (!m_active) goto done;
      auto item = std::move(m_notifications.front());
      m_notifications.pop();

      // Use index because iterator might get invalidated.
      for (size_t i = 0; i < m_listeners.size(); ++i) {
        if (!m_listeners[i]) continue;  // removed

        // Event type must be within requested set for this listener.
        if ((item.kind & m_listeners[i].eventMask) == 0) continue;

        // make a copy of the callback so we can safely release the mutex
        auto callback = m_listeners[i].callback;

        // Don't hold mutex during callback execution!
        lock.unlock();
        callback(item);
        lock.lock();
      }
    }
  }

done:
  if (m_on_exit) m_on_exit();
}

int Notifier::AddListener(std::function<void(const RawEvent& event)> callback,
                          int eventMask) {
  Start();
  auto thr = m_owner.GetThread();
  return thr->m_listeners.emplace_back(callback, eventMask);
}

void Notifier::RemoveListener(int uid) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  thr->m_listeners.erase(uid);
}

void Notifier::NotifySource(const wpi::Twine& name, CS_Source source,
                            CS_EventKind kind) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  thr->m_notifications.emplace(name, source, static_cast<RawEvent::Kind>(kind));
  thr->m_cond.notify_one();
}

void Notifier::NotifySource(const SourceImpl& source, CS_EventKind kind) {
  auto handleData = Instance::GetInstance().FindSource(source);
  NotifySource(source.GetName(), handleData.first, kind);
}

void Notifier::NotifySourceVideoMode(const SourceImpl& source,
                                     const VideoMode& mode) {
  auto thr = m_owner.GetThread();
  if (!thr) return;

  auto handleData = Instance::GetInstance().FindSource(source);

  thr->m_notifications.emplace(source.GetName(), handleData.first, mode);
  thr->m_cond.notify_one();
}

void Notifier::NotifySourceProperty(const SourceImpl& source, CS_EventKind kind,
                                    const wpi::Twine& propertyName,
                                    int property, CS_PropertyKind propertyKind,
                                    int value, const wpi::Twine& valueStr) {
  auto thr = m_owner.GetThread();
  if (!thr) return;

  auto handleData = Instance::GetInstance().FindSource(source);

  thr->m_notifications.emplace(
      propertyName, handleData.first, static_cast<RawEvent::Kind>(kind),
      Handle{handleData.first, property, Handle::kProperty}, propertyKind,
      value, valueStr);
  thr->m_cond.notify_one();
}

void Notifier::NotifySink(const wpi::Twine& name, CS_Sink sink,
                          CS_EventKind kind) {
  auto thr = m_owner.GetThread();
  if (!thr) return;

  thr->m_notifications.emplace(name, sink, static_cast<RawEvent::Kind>(kind));
  thr->m_cond.notify_one();
}

void Notifier::NotifySink(const SinkImpl& sink, CS_EventKind kind) {
  auto handleData = Instance::GetInstance().FindSink(sink);
  NotifySink(sink.GetName(), handleData.first, kind);
}

void Notifier::NotifySinkSourceChanged(const wpi::Twine& name, CS_Sink sink,
                                       CS_Source source) {
  auto thr = m_owner.GetThread();
  if (!thr) return;

  RawEvent event{name, sink, RawEvent::kSinkSourceChanged};
  event.sourceHandle = source;

  thr->m_notifications.emplace(std::move(event));
  thr->m_cond.notify_one();
}

void Notifier::NotifySinkProperty(const SinkImpl& sink, CS_EventKind kind,
                                  const wpi::Twine& propertyName, int property,
                                  CS_PropertyKind propertyKind, int value,
                                  const wpi::Twine& valueStr) {
  auto thr = m_owner.GetThread();
  if (!thr) return;

  auto handleData = Instance::GetInstance().FindSink(sink);

  thr->m_notifications.emplace(
      propertyName, handleData.first, static_cast<RawEvent::Kind>(kind),
      Handle{handleData.first, property, Handle::kSinkProperty}, propertyKind,
      value, valueStr);
  thr->m_cond.notify_one();
}

void Notifier::NotifyNetworkInterfacesChanged() {
  auto thr = m_owner.GetThread();
  if (!thr) return;

  thr->m_notifications.emplace(RawEvent::kNetworkInterfacesChanged);
  thr->m_cond.notify_one();
}

void Notifier::NotifyTelemetryUpdated() {
  auto thr = m_owner.GetThread();
  if (!thr) return;

  thr->m_notifications.emplace(RawEvent::kTelemetryUpdated);
  thr->m_cond.notify_one();
}
