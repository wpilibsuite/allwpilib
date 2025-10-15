// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_NETWORKLISTENER_H_
#define CSCORE_NETWORKLISTENER_H_

#include <memory>

#include <wpi/Logger.h>

namespace cs {

class Notifier;

class NetworkListener {
 public:
  NetworkListener(wpi::Logger& logger, Notifier& notifier);
  ~NetworkListener();

  void Start();
  void Stop();

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace cs

#endif  // CSCORE_NETWORKLISTENER_H_
