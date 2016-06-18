/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <functional>

#include "ErrorBase.h"
#include "HAL/Notifier.h"
#include "HAL/cpp/priority_mutex.h"

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
  static void Notify(uint64_t currentTimeInt, void* param);

  // held while updating process information
  priority_mutex m_processMutex;
  // HAL handle, atomic for proper destruction
  std::atomic<HalNotifierHandle> m_notifier{0};
  // address of the handler
  TimerEventHandler m_handler;
  // the absolute expiration time
  double m_expirationTime = 0;
  // the relative time (either periodic or single)
  double m_period = 0;
  // true if this is a periodic event
  bool m_periodic = false;

  // held HAL notifier while Notifier::Notify() call is in progress
  priority_mutex m_notifyMutex;
};
