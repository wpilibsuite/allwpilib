// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NetworkTablesSettings.h"

#include <optional>
#include <string_view>
#include <utility>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <ntcore_cpp.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>

#include "glass/Context.h"
#include "glass/Storage.h"

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

    int mode;
    bool dsClient;

    do {
      mode = m_mode;
      dsClient = m_dsClient;

      // release lock while stopping to avoid blocking GUI
      lock.unlock();

      // if just changing servers in client mode, no need to stop and restart
      unsigned int curMode = nt::GetNetworkMode(m_inst);
      if (mode != 1 || (curMode & NT_NET_MODE_SERVER) != 0) {
        nt::StopClient(m_inst);
        nt::StopServer(m_inst);
        nt::StopLocal(m_inst);
      }

      if (m_mode != 1 || !dsClient) {
        nt::StopDSClient(m_inst);
      }

      lock.lock();
    } while (mode != m_mode || dsClient != m_dsClient);

    if (m_mode == 1) {
      std::string_view serverTeam{m_serverTeam};
      std::optional<unsigned int> team;
      if (!wpi::contains(serverTeam, '.') &&
          (team = wpi::parse_integer<unsigned int>(serverTeam, 10))) {
        nt::StartClientTeam(m_inst, team.value(), NT_DEFAULT_PORT);
      } else {
        wpi::SmallVector<std::string_view, 4> serverNames;
        wpi::SmallVector<std::pair<std::string_view, unsigned int>, 4> servers;
        wpi::split(serverTeam, serverNames, ',', -1, false);
        for (auto&& serverName : serverNames) {
          servers.emplace_back(serverName, NT_DEFAULT_PORT);
        }
        nt::StartClient(m_inst, servers);
      }

      if (m_dsClient) {
        nt::StartDSClient(m_inst, NT_DEFAULT_PORT);
      }
    } else if (m_mode == 2) {
      nt::StartServer(m_inst, m_iniName.c_str(), m_listenAddress.c_str(),
                      NT_DEFAULT_PORT);
    }
  }
}

NetworkTablesSettings::NetworkTablesSettings(Storage& storage, NT_Inst inst)
    : m_mode{storage.GetString("mode"), 0, {"Disabled", "Client", "Server"}},
      m_iniName{storage.GetString("iniName", "networktables.ini")},
      m_serverTeam{storage.GetString("serverTeam")},
      m_listenAddress{storage.GetString("listenAddress")},
      m_dsClient{storage.GetBool("dsClient", true)} {
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
  thr->m_mode = m_mode.GetValue();
  thr->m_iniName = m_iniName;
  thr->m_serverTeam = m_serverTeam;
  thr->m_listenAddress = m_listenAddress;
  thr->m_dsClient = m_dsClient;
  thr->m_cond.notify_one();
}

bool NetworkTablesSettings::Display() {
  m_mode.Combo("Mode", m_serverOption ? 3 : 2);
  switch (m_mode.GetValue()) {
    case 1:
      ImGui::InputText("Team/IP", &m_serverTeam);
      ImGui::Checkbox("Get Address from DS", &m_dsClient);
      break;
    case 2:
      ImGui::InputText("Listen Address", &m_listenAddress);
      ImGui::InputText("ini Filename", &m_iniName);
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
