// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NetworkTablesSettings.hpp"

#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::glass;

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
      unsigned int curMode = wpi::nt::GetNetworkMode(m_inst);
      if ((mode == 0 || mode == 2) ||
          (mode == 1 && (curMode & NT_NET_MODE_CLIENT) == 0)) {
        wpi::nt::StopClient(m_inst);
        wpi::nt::StopServer(m_inst);
        wpi::nt::StopLocal(m_inst);
      }

      if ((m_mode == 0 || m_mode == 2) || !dsClient) {
        wpi::nt::StopDSClient(m_inst);
      }

      lock.lock();
    } while (mode != m_mode || dsClient != m_dsClient);

    if (m_mode == 1) {
      std::string_view serverTeam{m_serverTeam};
      std::optional<unsigned int> team;
      if (m_mode == 1) {
        wpi::nt::StartClient(m_inst, m_clientName);
      }

      if (!wpi::util::contains(serverTeam, '.') &&
          (team = wpi::util::parse_integer<unsigned int>(serverTeam, 10))) {
        wpi::nt::SetServerTeam(m_inst, team.value(), m_port);
      } else {
        std::vector<std::pair<std::string_view, unsigned int>> servers;
        wpi::util::split(serverTeam, ',', -1, false, [&](auto serverName) {
          servers.emplace_back(serverName, m_port);
        });
        wpi::nt::SetServer(m_inst, servers);
      }

      if (m_dsClient) {
        wpi::nt::StartDSClient(m_inst, m_port);
      }
    } else if (m_mode == 3) {
      wpi::nt::StartServer(m_inst, m_iniName.c_str(), m_listenAddress.c_str(),
                      m_port);
    }
  }
}

NetworkTablesSettings::NetworkTablesSettings(std::string_view clientName,
                                             Storage& storage, NT_Inst inst)
    : m_mode{storage.GetString("mode"), 0, {"Disabled", "Client", "Server"}},
      m_persistentFilename{
          storage.GetString("persistentFilename", "networktables.json")},
      m_serverTeam{storage.GetString("serverTeam")},
      m_listenAddress{storage.GetString("listenAddress")},
      m_clientName{storage.GetString("clientName", clientName)},
      m_port{storage.GetInt("port", NT_DEFAULT_PORT)},
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
  thr->m_iniName = m_persistentFilename;
  thr->m_serverTeam = m_serverTeam;
  thr->m_listenAddress = m_listenAddress;
  thr->m_clientName = m_clientName;
  thr->m_port = m_port;
  thr->m_dsClient = m_dsClient;
  thr->m_cond.notify_one();
}

static void LimitPortRange(int* port) {
  if (*port < 0) {
    *port = 0;
  } else if (*port > 65535) {
    *port = 65535;
  }
}

bool NetworkTablesSettings::Display() {
  m_mode.Combo("Mode", m_serverOption ? 3 : 2);
  switch (m_mode.GetValue()) {
    case 1:
      ImGui::InputText("Team/IP", &m_serverTeam);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip("Team number or IP/MDNS address of server");
      }
      if (ImGui::InputInt("Port", &m_port)) {
        LimitPortRange(&m_port);
      }
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip("TCP Port - leave this at the default");
      }
      ImGui::SameLine();
      if (ImGui::SmallButton("Default")) {
        m_port = NT_DEFAULT_PORT;
      }
      ImGui::InputText("Network Identity", &m_clientName);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip(
            "Arbitrary name used to identify clients on the network. Must not "
            "be blank.");
      }
      ImGui::Checkbox("Get Address from DS", &m_dsClient);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip("Attempt to fetch server IP from Driver Station");
      }
      break;
    case 2:
      ImGui::InputText("Listen Address", &m_listenAddress);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip(
            "Address for server to listen on. Leave blank to listen on all "
            "interfaces.");
      }
      if (ImGui::InputInt("Port", &m_port)) {
        LimitPortRange(&m_port);
      }
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip("TCP Port for NT4. Leave at default if unsure.");
      }
      ImGui::SameLine();
      if (ImGui::SmallButton("Default##default4")) {
        m_port = NT_DEFAULT_PORT;
      }
      ImGui::InputText("Persistent Filename", &m_persistentFilename);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip("File for storage of persistent entries");
      }
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
