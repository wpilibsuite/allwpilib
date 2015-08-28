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

  unsigned int AddEntryListener(StringRef prefix,
                                EntryListenerCallback callback);
  void RemoveEntryListener(unsigned int entry_listener_uid);

  void NotifyEntry(StringRef name, std::shared_ptr<Value> value, bool is_new);

  unsigned int AddConnectionListener(ConnectionListenerCallback callback);
  void RemoveConnectionListener(unsigned int conn_listener_uid);

  void NotifyConnection(bool connected, const ConnectionInfo& conn_info);

 private:
  Notifier();

  void ThreadMain();

  std::atomic_bool m_active;

  std::mutex m_mutex;
  std::condition_variable m_cond;
  std::vector<std::pair<std::string, EntryListenerCallback>> m_entry_listeners;
  std::vector<ConnectionListenerCallback> m_conn_listeners;

  struct EntryNotification {
    EntryNotification(StringRef name_, std::shared_ptr<Value> value_,
                      bool is_new_)
        : name(name_), value(value_), is_new(is_new_) {}

    std::string name;
    std::shared_ptr<Value> value;
    bool is_new;
  };
  std::queue<EntryNotification> m_entry_notifications;

  struct ConnectionNotification {
    ConnectionNotification(bool connected_, const ConnectionInfo& conn_info_)
        : connected(connected_), conn_info(conn_info_) {}

    bool connected;
    ConnectionInfo conn_info;
  };
  std::queue<ConnectionNotification> m_conn_notifications;

  std::thread m_thread;

  ATOMIC_STATIC_DECL(Notifier)
};

}  // namespace nt

#endif  // NT_NOTIFIER_H_
