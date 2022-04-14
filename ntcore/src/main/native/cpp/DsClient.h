// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
