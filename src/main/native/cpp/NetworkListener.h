/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_NETWORKLISTENER_H_
#define CSCORE_NETWORKLISTENER_H_

#include <support/SafeThread.h>

namespace cs {

class NetworkListener {
 public:
  static NetworkListener& GetInstance() {
    static NetworkListener instance;
    return instance;
  }
  ~NetworkListener();

  void Start();
  void Stop();

 private:
  NetworkListener() = default;

  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;
};

}  // namespace cs

#endif  // CSCORE_NETWORKLISTENER_H_
