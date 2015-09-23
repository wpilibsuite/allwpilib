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
  m_thread = std::thread(&Notifier::ThreadMain, this);
}

void Notifier::Stop() {
  m_active = false;
  // send notification so the thread terminates
  m_cond.notify_one();
  if (m_thread.joinable()) m_thread.join();
}

void Notifier::ThreadMain() {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_active) {
    while (m_entry_notifications.empty() && m_conn_notifications.empty()) {
      m_cond.wait(lock);
      if (!m_active) return;
    }

    // Entry notifications
    while (!m_entry_notifications.empty()) {
      auto item = std::move(m_entry_notifications.front());
      m_entry_notifications.pop();

      if (!item.value) continue;
      StringRef name(item.name);

      if (item.only) {
        // Don't hold mutex during callback execution!
        lock.unlock();
        item.only(0, name, item.value, item.is_new);
        lock.lock();
        continue;
      }

      // Use index because iterator might get invalidated.
      for (std::size_t i=0; i<m_entry_listeners.size(); ++i) {
        if (!m_entry_listeners[i].callback) continue;  // removed
        if (item.is_local && !m_entry_listeners[i].local_notify) continue;
        if (!name.startswith(m_entry_listeners[i].prefix)) continue;
        auto callback = m_entry_listeners[i].callback;
        // Don't hold mutex during callback execution!
        lock.unlock();
        callback(i+1, name, item.value, item.is_new);
        lock.lock();
      }
    }

    // Connection notifications
    while (!m_conn_notifications.empty()) {
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
}

unsigned int Notifier::AddEntryListener(StringRef prefix,
                                        EntryListenerCallback callback,
                                        bool local_notify) {
  std::lock_guard<std::mutex> lock(m_mutex);
  unsigned int uid = m_entry_listeners.size();
  m_entry_listeners.emplace_back(prefix, callback, local_notify);
  if (local_notify) m_local_notifiers = true;
  return uid + 1;
}

void Notifier::RemoveEntryListener(unsigned int entry_listener_uid) {
  --entry_listener_uid;
  std::lock_guard<std::mutex> lock(m_mutex);
  if (entry_listener_uid < m_entry_listeners.size())
    m_entry_listeners[entry_listener_uid].callback = nullptr;
}

void Notifier::NotifyEntry(StringRef name, std::shared_ptr<Value> value,
                           bool is_new, bool is_local,
                           EntryListenerCallback only) {
  if (!m_active) return;
  if (is_local && !m_local_notifiers) return;  // optimization
  std::unique_lock<std::mutex> lock(m_mutex);
  m_entry_notifications.emplace(name, value, is_new, is_local, only);
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
