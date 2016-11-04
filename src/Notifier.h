/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_NOTIFIER_H_
#define NT_NOTIFIER_H_

#include <functional>

#include "support/atomic_static.h"
#include "support/SafeThread.h"
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

  bool local_notifiers() const { return m_local_notifiers; }
  static bool destroyed() { return s_destroyed; }

  void SetOnStart(std::function<void()> on_start) { m_on_start = on_start; }
  void SetOnExit(std::function<void()> on_exit) { m_on_exit = on_exit; }

  unsigned int AddEntryListener(llvm::StringRef prefix,
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

  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;

  std::atomic_bool m_local_notifiers;

  std::function<void()> m_on_start;
  std::function<void()> m_on_exit;

  ATOMIC_STATIC_DECL(Notifier)
  static bool s_destroyed;
};

}  // namespace nt

#endif  // NT_NOTIFIER_H_
