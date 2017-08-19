/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_NETWORKLISTENER_H_
#define CS_NETWORKLISTENER_H_

#include "support/atomic_static.h"
#include "support/SafeThread.h"

namespace cs {

class NetworkListener {
 public:
  static NetworkListener& GetInstance() {
    ATOMIC_STATIC(NetworkListener, instance);
    return instance;
  }
  ~NetworkListener();

  void Start();
  void Stop();

 private:
  NetworkListener() = default;

  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;

  ATOMIC_STATIC_DECL(NetworkListener)
};

}  // namespace cs

#endif  // CS_NETWORKLISTENER_H_
