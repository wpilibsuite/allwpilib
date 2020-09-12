/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkTablesSettings.h"

#include <utility>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <ntcore_cpp.h>
#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

#include "glass/Context.h"

NetworkTablesSettings::NetworkTablesSettings(NT_Inst inst,
                                             const char* storageName)
    : m_inst{inst} {
  auto& storage = glass::GetStorage(storageName);
  m_pMode = storage.GetIntRef("mode");
  m_pIniName = storage.GetStringRef("iniName", "networktables.ini");
  m_pServerTeam = storage.GetStringRef("serverTeam");
  m_pListenAddress = storage.GetStringRef("listenAddress");
}

void NetworkTablesSettings::Update() {
  if (!m_restart) return;
  m_restart = false;
  nt::StopClient(m_inst);
  nt::StopServer(m_inst);
  nt::StopLocal(m_inst);
  if (*m_pMode == 1) {
    wpi::StringRef serverTeam{*m_pServerTeam};
    unsigned int team;
    if (!serverTeam.contains('.') && !serverTeam.getAsInteger(10, team)) {
      nt::StartClientTeam(m_inst, team, NT_DEFAULT_PORT);
    } else {
      wpi::SmallVector<wpi::StringRef, 4> serverNames;
      wpi::SmallVector<std::pair<wpi::StringRef, unsigned int>, 4> servers;
      serverTeam.split(serverNames, ',', -1, false);
      for (auto&& serverName : serverNames)
        servers.emplace_back(serverName, NT_DEFAULT_PORT);
      nt::StartClient(m_inst, servers);
    }
  } else if (*m_pMode == 2) {
    nt::StartServer(m_inst, m_pIniName->c_str(), m_pListenAddress->c_str(),
                    NT_DEFAULT_PORT);
  }
}

void NetworkTablesSettings::Display() {
  static const char* modeOptions[] = {"Disabled", "Client", "Server"};
  ImGui::Combo("Mode", m_pMode, modeOptions, 3);
  switch (*m_pMode) {
    case 1:
      ImGui::InputText("Team/IP", m_pServerTeam);
      break;
    case 2:
      ImGui::InputText("Listen Address", m_pListenAddress);
      ImGui::InputText("ini Filename", m_pIniName);
      break;
    default:
      break;
  }
  if (ImGui::Button("Apply")) m_restart = true;
}
