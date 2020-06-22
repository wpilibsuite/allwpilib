/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_NETWORKLISTENER_H_
#define CSCORE_NETWORKLISTENER_H_

#include <memory>

#include <wpi/Logger.h>
#include <wpi/Signal.h>

namespace cs {

class NetworkListener {
 public:
  explicit NetworkListener(wpi::Logger& logger);
  ~NetworkListener();

  void Start();
  void Stop();

  // Called when an interface changed
  wpi::sig::Signal<> interfacesChanged;

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace cs

#endif  // CSCORE_NETWORKLISTENER_H_
