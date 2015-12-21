/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Notifier.h"

using namespace nt;

ATOMIC_STATIC_INIT(Notifier)
bool Notifier::s_destroyed = false;

Notifier::Notifier() {
  m_active = false;
  m_local_notifiers = false;
  s_destroyed = false;
}

Notifier::~Notifier() {
  s_destroyed = true;
  Stop();
}

void Notifier::Start() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_active) return;
    m_active = true;
  }
  {
    std::lock_guard<std::mutex> lock(m_shutdown_mutex);
    m_shutdown = false;
  }
  m_thread = std::thread(&Notifier::ThreadMain, this);
}

void Notifier::Stop() {
  m_active = false;
  // send notification so the thread terminates
  m_cond.notify_one();
  if (m_thread.joinable()) {
    // join with timeout
    std::unique_lock<std::mutex> lock(m_shutdown_mutex);
    auto timeout_time =
        std::chrono::steady_clock::now() + std::chrono::seconds(1);
    if (m_shutdown_cv.wait_until(lock, timeout_time,
                                 [&] { return m_shutdown; }))
      m_thread.join();
    else
      m_thread.detach();  // timed out, detach it
  }
}

void Notifier::ThreadMain() {
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
      for (std::size_t i=0; i<m_entry_listeners.size(); ++i) {
        if (!m_entry_listeners[i].callback) continue;  // removed

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
        callback(i+1, name, item.value, item.flags);
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
      for (std::size_t i=0; i<m_conn_listeners.size(); ++i) {
        if (!m_conn_listeners[i]) continue;  // removed
        auto callback = m_conn_listeners[i];
        // Don't hold mutex during callback execution!
        lock.unlock();
        callback(i+1, item.connected, item.conn_info);
        lock.lock();
      }
    }
  }

done:
  if (m_on_exit) m_on_exit();

  // use condition variable to signal thread shutdown
  {
    std::lock_guard<std::mutex> lock(m_shutdown_mutex);
    m_shutdown = true;
    m_shutdown_cv.notify_one();
  }
}

unsigned int Notifier::AddEntryListener(StringRef prefix,
                                        EntryListenerCallback callback,
                                        unsigned int flags) {
  std::lock_guard<std::mutex> lock(m_mutex);
  unsigned int uid = m_entry_listeners.size();
  m_entry_listeners.emplace_back(prefix, callback, flags);
  if ((flags & NT_NOTIFY_LOCAL) != 0) m_local_notifiers = true;
  return uid + 1;
}

void Notifier::RemoveEntryListener(unsigned int entry_listener_uid) {
  --entry_listener_uid;
  std::lock_guard<std::mutex> lock(m_mutex);
  if (entry_listener_uid < m_entry_listeners.size())
    m_entry_listeners[entry_listener_uid].callback = nullptr;
}

void Notifier::NotifyEntry(StringRef name, std::shared_ptr<Value> value,
                           unsigned int flags, EntryListenerCallback only) {
  if (!m_active) return;
  // optimization: don't generate needless local queue entries if we have
  // no local listeners (as this is a common case on the server side)
  if ((flags & NT_NOTIFY_LOCAL) != 0 && !m_local_notifiers) return;
  std::unique_lock<std::mutex> lock(m_mutex);
  m_entry_notifications.emplace(name, value, flags, only);
  lock.unlock();
  m_cond.notify_one();
}

unsigned int Notifier::AddConnectionListener(
    ConnectionListenerCallback callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  unsigned int uid = m_entry_listeners.size();
  m_conn_listeners.emplace_back(callback);
  return uid + 1;
}

void Notifier::RemoveConnectionListener(unsigned int conn_listener_uid) {
  --conn_listener_uid;
  std::lock_guard<std::mutex> lock(m_mutex);
  if (conn_listener_uid < m_conn_listeners.size())
    m_conn_listeners[conn_listener_uid] = nullptr;
}

void Notifier::NotifyConnection(bool connected,
                                const ConnectionInfo& conn_info,
                                ConnectionListenerCallback only) {
  if (!m_active) return;
  std::unique_lock<std::mutex> lock(m_mutex);
  m_conn_notifications.emplace(connected, conn_info, only);
  lock.unlock();
  m_cond.notify_one();
}
