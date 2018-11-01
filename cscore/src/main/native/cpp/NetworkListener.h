/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_NETWORKLISTENER_H_
#define CSCORE_NETWORKLISTENER_H_

#include <wpi/Logger.h>
#include <wpi/SafeThread.h>

namespace cs {

class Notifier;

class NetworkListener {
 public:
  NetworkListener(wpi::Logger& logger, Notifier& notifier)
      : m_logger(logger), m_notifier(notifier) {}
  ~NetworkListener();

  void Start();
  void Stop();

 private:
  wpi::Logger& m_logger;
  Notifier& m_notifier;

  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;
};

}  // namespace cs

#endif  // CSCORE_NETWORKLISTENER_H_
