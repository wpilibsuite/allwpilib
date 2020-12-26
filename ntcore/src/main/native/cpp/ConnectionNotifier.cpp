// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
