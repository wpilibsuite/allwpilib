// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NetworkTablesHelper.h"

using namespace glass;

NetworkTablesHelper::NetworkTablesHelper(NT_Inst inst)
    : m_inst{inst},
      m_topicPoller{nt::CreateTopicListenerPoller(inst)},
      m_valuePoller{nt::CreateValueListenerPoller(inst)} {}

NetworkTablesHelper::~NetworkTablesHelper() {
  nt::DestroyTopicListenerPoller(m_topicPoller);
  nt::DestroyValueListenerPoller(m_valuePoller);
}

bool NetworkTablesHelper::IsConnected() const {
  return nt::GetNetworkMode(m_inst) == NT_NET_MODE_SERVER ||
         nt::IsConnected(m_inst);
}
