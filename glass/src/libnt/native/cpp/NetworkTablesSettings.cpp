// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NetworkTablesSettings.h"

#include <utility>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <ntcore_cpp.h>
#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

#include "glass/Context.h"

using namespace glass;

void NetworkTablesSettings::Thread::Main() {
  while (m_active) {
    // wait to be woken up
    std::unique_lock lock(m_mutex);
    m_cond.wait(lock, [&] { return !m_active || m_restart; });
    if (!m_active) {
      break;
    }

    // clear restart flag
    m_restart = false;

    // if just changing servers in client mode, no need to stop and restart
    unsigned int curMode = nt::GetNetworkMode(m_inst);

    if (m_mode != 1 || (curMode & NT_NET_MODE_SERVER) != 0) {
      // release lock while stopping to avoid blocking GUI
      lock.unlock();
      nt::StopClient(m_inst);
      nt::StopServer(m_inst);
      nt::StopLocal(m_inst);
      lock.lock();
    }

    if (m_mode == 1) {
      wpi::StringRef serverTeam{m_serverTeam};
      unsigned int team;
      if (!serverTeam.contains('.') && !serverTeam.getAsInteger(10, team)) {
        nt::StartClientTeam(m_inst, team, NT_DEFAULT_PORT);
      } else {
        wpi::SmallVector<wpi::StringRef, 4> serverNames;
        wpi::SmallVector<std::pair<wpi::StringRef, unsigned int>, 4> servers;
        serverTeam.split(serverNames, ',', -1, false);
        for (auto&& serverName : serverNames) {
          servers.emplace_back(serverName, NT_DEFAULT_PORT);
        }
        nt::StartClient(m_inst, servers);
      }
    } else if (m_mode == 2) {
      nt::StartServer(m_inst, m_iniName.c_str(), m_listenAddress.c_str(),
                      NT_DEFAULT_PORT);
    }
  }
}

NetworkTablesSettings::NetworkTablesSettings(NT_Inst inst,
                                             const char* storageName) {
  auto& storage = glass::GetStorage(storageName);
  m_pMode = storage.GetIntRef("mode");
  m_pIniName = storage.GetStringRef("iniName", "networktables.ini");
  m_pServerTeam = storage.GetStringRef("serverTeam");
  m_pListenAddress = storage.GetStringRef("listenAddress");

  m_thread.Start(inst);
}

void NetworkTablesSettings::Update() {
  if (!m_restart) {
    return;
  }
  m_restart = false;

  // do actual operation on thread
  auto thr = m_thread.GetThread();
  thr->m_restart = true;
  thr->m_mode = *m_pMode;
  thr->m_iniName = *m_pIniName;
  thr->m_serverTeam = *m_pServerTeam;
  thr->m_listenAddress = *m_pListenAddress;
  thr->m_cond.notify_one();
}

bool NetworkTablesSettings::Display() {
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
  if (ImGui::Button("Apply")) {
    m_restart = true;
    return true;
  }
  return false;
}
