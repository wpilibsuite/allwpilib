/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ConnectionNotifier.h"

using namespace nt;

ConnectionNotifier::ConnectionNotifier(int inst) : m_inst(inst) {}

void ConnectionNotifier::Start() { DoStart(m_inst); }

unsigned int ConnectionNotifier::Add(
    std::function<void(const ConnectionNotification& event)> callback) {
  return DoAdd(callback);
}

unsigned int ConnectionNotifier::AddPolled(unsigned int poller_uid) {
  return DoAdd(poller_uid);
}

void ConnectionNotifier::NotifyConnection(bool connected,
                                          const ConnectionInfo& conn_info,
                                          unsigned int only_listener) {
  Send(only_listener, 0, connected, conn_info);
}
