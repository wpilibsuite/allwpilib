/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NetworkTablesHelper.h"

using namespace glass;

NetworkTablesHelper::NetworkTablesHelper(NT_Inst inst)
    : m_inst{inst}, m_poller{nt::CreateEntryListenerPoller(inst)} {}

NetworkTablesHelper::~NetworkTablesHelper() {
  nt::DestroyEntryListenerPoller(m_poller);
}

bool NetworkTablesHelper::IsConnected() const {
  return nt::GetNetworkMode(m_inst) == NT_NET_MODE_SERVER ||
         nt::IsConnected(m_inst);
}
