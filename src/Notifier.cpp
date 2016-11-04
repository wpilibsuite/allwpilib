/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Notifier.h"

#include <queue>
#include <vector>

using namespace nt;

ATOMIC_STATIC_INIT(Notifier)
bool Notifier::s_destroyed = false;

namespace {
// Vector which provides an integrated freelist for removal and reuse of
// individual elements.
template <typename T>
class UidVector {
 public:
  typedef typename std::vector<T>::size_type size_type;

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

}  // anonymous namespace

class Notifier::Thread : public wpi::SafeThread {
 public:
  Thread(std::function<void()> on_start, std::function<void()> on_exit)
      : m_on_start(on_start), m_on_exit(on_exit) {}

  void Main();

  struct EntryListener {
    EntryListener() = default;
    EntryListener(StringRef prefix_, EntryListenerCallback callback_,
                  unsigned int flags_)
        : prefix(prefix_), callback(callback_), flags(flags_) {}

    explicit operator bool() const { return bool(callback); }

    std::string prefix;
    EntryListenerCallback callback;
    unsigned int flags;
  };
  UidVector<EntryListener> m_entry_listeners;
  UidVector<ConnectionListenerCallback> m_conn_listeners;

  struct EntryNotification {
    EntryNotification(llvm::StringRef name_, std::shared_ptr<Value> value_,
                      unsigned int flags_, EntryListenerCallback only_)
        : name(name_), value(value_), flags(flags_), only(only_) {}

    std::string name;
    std::shared_ptr<Value> value;
    unsigned int flags;
    EntryListenerCallback only;
  };
  std::queue<EntryNotification> m_entry_notifications;

  struct ConnectionNotification {
    ConnectionNotification(bool connected_, const ConnectionInfo& conn_info_,
                           ConnectionListenerCallback only_)
        : connected(connected_), conn_info(conn_info_), only(only_) {}

    bool connected;
    ConnectionInfo conn_info;
    ConnectionListenerCallback only;
  };
  std::queue<ConnectionNotification> m_conn_notifications;

  std::function<void()> m_on_start;
  std::function<void()> m_on_exit;
};

Notifier::Notifier() {
  m_local_notifiers = false;
  s_destroyed = false;
}

Notifier::~Notifier() { s_destroyed = true; }

void Notifier::Start() {
  auto thr = m_owner.GetThread();
  if (!thr) m_owner.Start(new Thread(m_on_start, m_on_exit));
}

void Notifier::Stop() { m_owner.Stop(); }

void Notifier::Thread::Main() {
  if (m_on_start) m_on_start();

  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_active) {
    while (m_entry_notifications.empty() && m_conn_notifications.empty()) {
      m_cond.wait(lock);
      if (!m_active) goto done;
    }

    // Entry notifications
    while (!m_entry_notifications.empty()) {
      if (!m_active) goto done;
      auto item = std::move(m_entry_notifications.front());
      m_entry_notifications.pop();

      if (!item.value) continue;
      StringRef name(item.name);

      if (item.only) {
        // Don't hold mutex during callback execution!
        lock.unlock();
        item.only(0, name, item.value, item.flags);
        lock.lock();
        continue;
      }

      // Use index because iterator might get invalidated.
      for (std::size_t i = 0; i < m_entry_listeners.size(); ++i) {
        if (!m_entry_listeners[i]) continue;  // removed

        // Flags must be within requested flag set for this listener.
        // Because assign messages can result in both a value and flags update,
        // we handle that case specially.
        unsigned int listen_flags = m_entry_listeners[i].flags;
        unsigned int flags = item.flags;
        unsigned int assign_both = NT_NOTIFY_UPDATE | NT_NOTIFY_FLAGS;
        if ((flags & assign_both) == assign_both) {
          if ((listen_flags & assign_both) == 0) continue;
          listen_flags &= ~assign_both;
          flags &= ~assign_both;
        }
        if ((flags & ~listen_flags) != 0) continue;

        // must match prefix
        if (!name.startswith(m_entry_listeners[i].prefix)) continue;

        // make a copy of the callback so we can safely release the mutex
        auto callback = m_entry_listeners[i].callback;

        // Don't hold mutex during callback execution!
        lock.unlock();
        callback(i + 1, name, item.value, item.flags);
        lock.lock();
      }
    }

    // Connection notifications
    while (!m_conn_notifications.empty()) {
      if (!m_active) goto done;
      auto item = std::move(m_conn_notifications.front());
      m_conn_notifications.pop();

      if (item.only) {
        // Don't hold mutex during callback execution!
        lock.unlock();
        item.only(0, item.connected, item.conn_info);
        lock.lock();
        continue;
      }

      // Use index because iterator might get invalidated.
      for (std::size_t i = 0; i < m_conn_listeners.size(); ++i) {
        if (!m_conn_listeners[i]) continue;  // removed
        auto callback = m_conn_listeners[i];
        // Don't hold mutex during callback execution!
        lock.unlock();
        callback(i + 1, item.connected, item.conn_info);
        lock.lock();
      }
    }
  }

done:
  if (m_on_exit) m_on_exit();
}

unsigned int Notifier::AddEntryListener(StringRef prefix,
                                        EntryListenerCallback callback,
                                        unsigned int flags) {
  Start();
  auto thr = m_owner.GetThread();
  if ((flags & NT_NOTIFY_LOCAL) != 0) m_local_notifiers = true;
  return thr->m_entry_listeners.emplace_back(prefix, callback, flags);
}

void Notifier::RemoveEntryListener(unsigned int entry_listener_uid) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  thr->m_entry_listeners.erase(entry_listener_uid);
}

void Notifier::NotifyEntry(StringRef name, std::shared_ptr<Value> value,
                           unsigned int flags, EntryListenerCallback only) {
  // optimization: don't generate needless local queue entries if we have
  // no local listeners (as this is a common case on the server side)
  if ((flags & NT_NOTIFY_LOCAL) != 0 && !m_local_notifiers) return;
  auto thr = m_owner.GetThread();
  if (!thr) return;
  thr->m_entry_notifications.emplace(name, value, flags, only);
  thr->m_cond.notify_one();
}

unsigned int Notifier::AddConnectionListener(
    ConnectionListenerCallback callback) {
  Start();
  auto thr = m_owner.GetThread();
  return thr->m_conn_listeners.emplace_back(callback);
}

void Notifier::RemoveConnectionListener(unsigned int conn_listener_uid) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  thr->m_conn_listeners.erase(conn_listener_uid);
}

void Notifier::NotifyConnection(bool connected, const ConnectionInfo& conn_info,
                                ConnectionListenerCallback only) {
  auto thr = m_owner.GetThread();
  if (!thr) return;
  thr->m_conn_notifications.emplace(connected, conn_info, only);
  thr->m_cond.notify_one();
}
