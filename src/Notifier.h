/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_NOTIFIER_H_
#define CS_NOTIFIER_H_

#include <functional>

#include "support/atomic_static.h"
#include "support/SafeThread.h"
#include "cscore_cpp.h"

namespace cs {

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

  static bool destroyed() { return s_destroyed; }

  void SetOnStart(std::function<void()> on_start) { m_on_start = on_start; }
  void SetOnExit(std::function<void()> on_exit) { m_on_exit = on_exit; }

  int AddListener(std::function<void(const RawEvent& event)> callback,
                  int eventMask);
  void RemoveListener(int uid);

  // Notification events
  void NotifySource(llvm::StringRef name, CS_Source source,
                    RawEvent::Type type);
  void NotifySourceVideoMode(llvm::StringRef name, CS_Source source,
                             const VideoMode& mode);
  void NotifySourceProperty(llvm::StringRef name, CS_Source source,
                            RawEvent::Type type, int property,
                            CS_PropertyType propertyType, int value,
                            llvm::StringRef valueStr);
  void NotifySink(llvm::StringRef name, CS_Sink sink, RawEvent::Type type);

 private:
  Notifier();

  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;

  std::function<void()> m_on_start;
  std::function<void()> m_on_exit;

  ATOMIC_STATIC_DECL(Notifier)
  static bool s_destroyed;
};

}  // namespace cs

#endif  // CS_NOTIFIER_H_
