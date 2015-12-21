/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_NOTIFIER_H_
#define NT_NOTIFIER_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "atomic_static.h"
#include "ntcore_cpp.h"

namespace nt {

class Notifier {
  friend class NotifierTest;
 public:
  static Notifier& GetInstance() {
    ATOMIC_STATIC(Notifier, instance);
    return instance;
  }
  ~Notifier();

  void Start();
  void Stop();

  bool active() const { return m_active; }
  bool local_notifiers() const { return m_local_notifiers; }
  static bool destroyed() { return s_destroyed; }

  void SetOnStart(std::function<void()> on_start) { m_on_start = on_start; }
  void SetOnExit(std::function<void()> on_exit) { m_on_exit = on_exit; }

  unsigned int AddEntryListener(StringRef prefix,
                                EntryListenerCallback callback,
                                unsigned int flags);
  void RemoveEntryListener(unsigned int entry_listener_uid);

  void NotifyEntry(StringRef name, std::shared_ptr<Value> value,
                   unsigned int flags, EntryListenerCallback only = nullptr);

  unsigned int AddConnectionListener(ConnectionListenerCallback callback);
  void RemoveConnectionListener(unsigned int conn_listener_uid);

  void NotifyConnection(bool connected, const ConnectionInfo& conn_info,
                        ConnectionListenerCallback only = nullptr);

 private:
  Notifier();

  void ThreadMain();

  std::atomic_bool m_active;
  std::atomic_bool m_local_notifiers;

  std::mutex m_mutex;
  std::condition_variable m_cond;

  struct EntryListener {
    EntryListener(StringRef prefix_, EntryListenerCallback callback_,
                  unsigned int flags_)
        : prefix(prefix_), callback(callback_), flags(flags_) {}

    std::string prefix;
    EntryListenerCallback callback;
    unsigned int flags;
  };
  std::vector<EntryListener> m_entry_listeners;
  std::vector<ConnectionListenerCallback> m_conn_listeners;

  struct EntryNotification {
    EntryNotification(StringRef name_, std::shared_ptr<Value> value_,
                      unsigned int flags_, EntryListenerCallback only_)
        : name(name_),
          value(value_),
          flags(flags_),
          only(only_) {}

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

  std::thread m_thread;
  std::mutex m_shutdown_mutex;
  std::condition_variable m_shutdown_cv;
  bool m_shutdown = false;

  std::function<void()> m_on_start;
  std::function<void()> m_on_exit;

  ATOMIC_STATIC_DECL(Notifier)
  static bool s_destroyed;
};

}  // namespace nt

#endif  // NT_NOTIFIER_H_
