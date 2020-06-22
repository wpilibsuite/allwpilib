/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_NOTIFIER_H_
#define CSCORE_NOTIFIER_H_

#include <functional>

#include <wpi/SafeThread.h>

#include "cscore_cpp.h"

namespace cs {

class Notifier {
  friend class NotifierTest;

 public:
  Notifier();
  ~Notifier();

  void Start();
  void Stop();

  static bool destroyed() { return s_destroyed; }

  void SetOnStart(std::function<void()> on_start) { m_on_start = on_start; }
  void SetOnExit(std::function<void()> on_exit) { m_on_exit = on_exit; }

  int AddListener(std::function<void(const RawEvent& event)> callback,
                  int eventMask);
  void RemoveListener(int uid);

  void Notify(RawEvent&& event);
  void Notify(RawEvent::Kind eventKind) { Notify(RawEvent{eventKind}); }

 private:
  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;

  std::function<void()> m_on_start;
  std::function<void()> m_on_exit;
  static bool s_destroyed;
};

}  // namespace cs

#endif  // CSCORE_NOTIFIER_H_
