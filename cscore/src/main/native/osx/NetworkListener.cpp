/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkListener.h"

#include "Log.h"
#include "Notifier.h"

using namespace cs;

class NetworkListener::Thread : public wpi::SafeThread {
 public:
  void Main();
};

NetworkListener::~NetworkListener() { Stop(); }

void NetworkListener::Start() {
  auto thr = m_owner.GetThread();
  if (!thr) m_owner.Start();
}

void NetworkListener::Stop() {
  // Wake up thread
  if (auto thr = m_owner.GetThread()) {
    thr->m_active = false;
  }
  m_owner.Stop();
}

void NetworkListener::Thread::Main() {
  // TODO
}
