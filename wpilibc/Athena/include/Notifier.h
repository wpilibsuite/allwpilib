/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "HAL/cpp/priority_mutex.h"

typedef void (*TimerEventHandler)(void *param);

class Notifier : public ErrorBase {
 public:
  Notifier(TimerEventHandler handler, void *param = nullptr);
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
  static void Notify(uint32_t currentTimeInt, void *param);

  // held while updating process information
  priority_mutex m_processMutex;
  // HAL handle
  void *m_notifier;
  // address of the handler
  TimerEventHandler m_handler;
  // a parameter to pass to the handler
  void *m_param;
  // the absolute expiration time
  double m_expirationTime = 0;
  // the relative time (either periodic or single)
  double m_period = 0;
  // true if this is a periodic event
  bool m_periodic = false;

  // held by interrupt manager task while handler call is in progress
  priority_mutex m_handlerMutex;
};
