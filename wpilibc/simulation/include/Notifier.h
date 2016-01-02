/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "HAL/cpp/priority_mutex.h"
#include <thread>
#include <atomic>

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
  static Notifier *timerQueueHead;
  static priority_recursive_mutex queueMutex;
  static priority_mutex halMutex;
  static void *m_notifier;
  static std::atomic<int> refcount;

  // process the timer queue on a timer event
  static void ProcessQueue(uint32_t mask, void *params);
  // update the FPGA alarm since the queue has changed
  static void UpdateAlarm();
  // insert this Notifier in the timer queue
  void InsertInQueue(bool reschedule);
  // delete this Notifier from the timer queue
  void DeleteFromQueue();

  // address of the handler
  TimerEventHandler m_handler;
  // a parameter to pass to the handler
  void *m_param;
  // the relative time (either periodic or single)
  double m_period = 0;
  // absolute expiration time for the current event
  double m_expirationTime = 0;
  // next Nofifier event
  Notifier *m_nextEvent = nullptr;
  // true if this is a periodic event
  bool m_periodic = false;
  // indicates if this entry is queued
  bool m_queued = false;
  // held by interrupt manager task while handler call is in progress
  priority_mutex m_handlerMutex;

  static std::thread m_task;
  static std::atomic<bool> m_stopped;
  static void Run();
};
