/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
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

  static void ProcessQueue(
      uint32_t mask, void *params);  // process the timer queue on a timer event
  static void
  UpdateAlarm();  // update the FPGA alarm since the queue has changed
  void InsertInQueue(
      bool reschedule);         // insert this Notifier in the timer queue
  void DeleteFromQueue();       // delete this Notifier from the timer queue
  TimerEventHandler m_handler;  // address of the handler
  void *m_param;                // a parameter to pass to the handler
  double m_period = 0;              // the relative time (either periodic or single)
  double m_expirationTime = 0;  // absolute expiration time for the current event
  Notifier *m_nextEvent = nullptr;    // next Nofifier event
  bool m_periodic = false;          // true if this is a periodic event
  bool m_queued = false;            // indicates if this entry is queued
  priority_mutex m_handlerMutex;  // held by interrupt manager task while
                                    // handler call is in progress

#ifdef FRC_SIMULATOR
  static std::thread m_task;
  static std::atomic<bool> m_stopped;
#endif
  static void Run();
};
