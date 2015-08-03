/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Notifier.h"

using namespace nt;

ATOMIC_STATIC_INIT(Notifier)

Notifier::Notifier() {
  m_active = false;
}

Notifier::~Notifier() { Stop(); }

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
  NotifyEntry("", nullptr, false);
  if (m_thread.joinable()) m_thread.join();
}

void Notifier::ThreadMain() {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_active) {
    m_cond.wait(lock);
    if (!m_active) continue;

    // Entry notifications
    while (!m_entry_notifications.empty()) {
      auto item = std::move(m_entry_notifications.front());
      m_entry_notifications.pop();

      if (!item.value) continue;
      StringRef name(item.name);

      // Use index because iterator might get invalidated.
      for (std::size_t i=0; i<m_entry_listeners.size(); ++i) {
        if (!m_entry_listeners[i].second) continue;  // removed
        if (!name.startswith(m_entry_listeners[i].first)) continue;
        auto callback = m_entry_listeners[i].second;
        // Don't hold mutex during callback execution!
        lock.unlock();
        callback(i, name, item.value, item.is_new);
        lock.lock();
      }
    }

    // Connection notifications
    while (!m_conn_notifications.empty()) {
      auto item = std::move(m_conn_notifications.front());
      m_conn_notifications.pop();

      // Use index because iterator might get invalidated.
      for (std::size_t i=0; i<m_conn_listeners.size(); ++i) {
        if (!m_conn_listeners[i]) continue;  // removed
        auto callback = m_conn_listeners[i];
        // Don't hold mutex during callback execution!
        lock.unlock();
        callback(i, item.connected, item.conn_info);
        lock.lock();
      }
    }
  }
}

unsigned int Notifier::AddEntryListener(StringRef prefix,
                                        EntryListenerCallback callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  unsigned int uid = m_entry_listeners.size();
  m_entry_listeners.emplace_back(prefix, callback);
  return uid;
}

void Notifier::RemoveEntryListener(unsigned int entry_listener_uid) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (entry_listener_uid < m_entry_listeners.size())
    m_entry_listeners[entry_listener_uid].second = nullptr;
}

void Notifier::NotifyEntry(StringRef name, std::shared_ptr<Value> value,
                           bool is_new) {
  if (!m_active) return;
  std::unique_lock<std::mutex> lock(m_mutex);
  m_entry_notifications.emplace(name, value, is_new);
  lock.unlock();
  m_cond.notify_one();
}

unsigned int Notifier::AddConnectionListener(
    ConnectionListenerCallback callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  unsigned int uid = m_entry_listeners.size();
  m_conn_listeners.emplace_back(callback);
  return uid;
}

void Notifier::RemoveConnectionListener(unsigned int conn_listener_uid) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (conn_listener_uid < m_conn_listeners.size())
    m_conn_listeners[conn_listener_uid] = nullptr;
}

void Notifier::NotifyConnection(int connected,
                                const ConnectionInfo& conn_info) {
  if (!m_active) return;
  std::unique_lock<std::mutex> lock(m_mutex);
  m_conn_notifications.emplace(connected, conn_info);
  lock.unlock();
  m_cond.notify_one();
}
