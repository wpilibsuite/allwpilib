/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_DSCLIENT_H_
#define NT_DSCLIENT_H_

#include "support/atomic_static.h"
#include "support/SafeThread.h"

namespace nt {

class DsClient {
 public:
  static DsClient& GetInstance() {
    ATOMIC_STATIC(DsClient, instance);
    return instance;
  }
  ~DsClient() = default;

  void Start(unsigned int port);
  void Stop();

 private:
  DsClient() = default;

  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;

  ATOMIC_STATIC_DECL(DsClient)
};

}  // namespace nt

#endif  // NT_DSCLIENT_H_
