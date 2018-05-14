/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_DSCLIENT_H_
#define NTCORE_DSCLIENT_H_

#include <wpi/SafeThread.h>

#include "Log.h"

namespace nt {

class Dispatcher;

class DsClient {
 public:
  DsClient(Dispatcher& dispatcher, wpi::Logger& logger);
  ~DsClient() = default;

  void Start(unsigned int port);
  void Stop();

 private:
  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;
  Dispatcher& m_dispatcher;
  wpi::Logger& m_logger;
};

}  // namespace nt

#endif  // NTCORE_DSCLIENT_H_
