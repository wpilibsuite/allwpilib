/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <atomic>
#include <functional>
#include <utility>

#include <HAL/Notifier.h>
#include <HAL/cpp/priority_mutex.h>

#include "ErrorBase.h"

namespace frc {

typedef std::function<void()> TimerEventHandler;

class Notifier : public ErrorBase {
 public:
  explicit Notifier(TimerEventHandler handler);

  template <typename Callable, typename Arg, typename... Args>
  Notifier(Callable&& f, Arg&& arg, Args&&... args)
      : Notifier(std::bind(std::forward<Callable>(f), std::forward<Arg>(arg),
                           std::forward<Args>(args)...)) {}

  virtual ~Notifier();

  Notifier(const Notifier&) = delete;
  Notifier& operator=(const Notifier&) = delete;

  void StartSingle(double delay);
  void StartPeriodic(double period);
  void Stop();

 private:
  // update the HAL alarm
  void UpdateAlarm();
  // HAL callback
  static void Notify(uint64_t currentTimeInt, HAL_NotifierHandle handle);

  // used to constrain execution between destructors and callback
  static hal::priority_mutex m_destructorMutex;
  // held while updating process information
  hal::priority_mutex m_processMutex;
  // HAL handle, atomic for proper destruction
  std::atomic<HAL_NotifierHandle> m_notifier{0};
  // address of the handler
  TimerEventHandler m_handler;
  // the absolute expiration time
  double m_expirationTime = 0;
  // the relative time (either periodic or single)
  double m_period = 0;
  // true if this is a periodic event
  bool m_periodic = false;
};

}  // namespace frc
