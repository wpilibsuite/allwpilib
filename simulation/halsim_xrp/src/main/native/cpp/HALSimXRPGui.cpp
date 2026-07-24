// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/HALSimXRPGui.hpp"

#include <algorithm>
#include <chrono>
#include <cinttypes>
#include <cstdio>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <imgui.h>

#include "wpi/hal/Extensions.h"
#include "wpi/halsim/xrp/HALSimXRP.hpp"
#include "wpi/net/BluetoothLEPacketClient.hpp"

using namespace wpilibxrp;
using namespace std::chrono_literals;

namespace {

constexpr std::string_view ADD_GUI_LATE_EXECUTE_NAME =
    "halsimgui::AddGuiLateExecute";
constexpr std::string_view GET_IMGUI_CONTEXT_NAME =
    "halsimgui::GetImguiContext";
constexpr std::string_view XRP_DEVICE_NAME_PREFIX = "WPIXRP-";

using AddGuiLateExecuteFn = void (*)(std::function<void()> execute);
using GetImguiContextFn = ImGuiContext* (*)();

struct CommandResult {
  int exitCode = -1;
  std::string output;
  std::vector<wpi::net::BluetoothLEDeviceInfo> devices;
  std::string targetAddress;
  XRPBluetoothAddressType addressType = XRPBluetoothAddressType::RANDOM;
  bool rememberTarget = false;
};

enum class CommandKind { NONE, REFRESH, SCAN, PAIR };

struct GuiState {
  bool initializedFromConnection = false;
  char address[128] = "";
  int addressType = 1;
  int selectedDevice = -1;
  std::vector<wpi::net::BluetoothLEDeviceInfo> devices;
  std::future<CommandResult> pendingCommand;
  CommandKind pendingKind = CommandKind::NONE;
  std::string commandStatus;
  std::string commandOutput;
};

static std::weak_ptr<HALSimXRP> gSimXRP;
static GetImguiContextFn gGetImguiContext = nullptr;
static bool gListenerRegistered = false;
static bool gLateExecuteRegistered = false;
static GuiState gGui;

static void SetAddress(std::string_view address) {
  std::snprintf(gGui.address, sizeof(gGui.address), "%.*s",
                static_cast<int>(address.size()), address.data());
}

static bool IsXRPDevice(const wpi::net::BluetoothLEDeviceInfo& device) {
  return device.name.rfind(XRP_DEVICE_NAME_PREFIX, 0) == 0;
}

static std::string_view GetDeviceSortKey(
    const wpi::net::BluetoothLEDeviceInfo& device) {
  return device.name.empty() ? std::string_view{device.target} : device.name;
}

static void FilterAndSortXRPDevices(
    std::vector<wpi::net::BluetoothLEDeviceInfo>* devices) {
  std::erase_if(*devices,
                [](const auto& device) { return !IsXRPDevice(device); });
  std::stable_sort(devices->begin(), devices->end(),
                   [](const auto& a, const auto& b) {
                     auto aKey = GetDeviceSortKey(a);
                     auto bKey = GetDeviceSortKey(b);
                     if (aKey != bKey) {
                       return aKey < bKey;
                     }
                     return a.target < b.target;
                   });
}

static CommandResult ScanDevices(std::chrono::milliseconds timeout) {
  CommandResult result;
  auto scan = wpi::net::BluetoothLEPacketClient::ScanDevices(timeout);
  result.devices = std::move(scan.devices);
  FilterAndSortXRPDevices(&result.devices);
  result.exitCode = scan.error.empty() ? 0 : 1;
  result.output =
      scan.error.empty() ? std::move(scan.status) : std::move(scan.error);
  return result;
}

static CommandResult RefreshDevices() {
  return ScanDevices(0ms);
}

static CommandResult PairDevice(std::string_view target,
                                XRPBluetoothAddressType addressType) {
  CommandResult result;
  auto pairing = wpi::net::BluetoothLEPacketClient::PairDevice(target);
  result.exitCode = pairing.paired ? 0 : 1;
  result.output = pairing.error.empty() ? std::move(pairing.status)
                                        : std::move(pairing.error);
  if (pairing.paired) {
    result.targetAddress = target;
    result.addressType = addressType;
    result.rememberTarget = true;
  }
  return result;
}

static void StartCommand(CommandKind kind, std::string status,
                         std::function<CommandResult()> command) {
  if (gGui.pendingCommand.valid()) {
    return;
  }

  gGui.pendingKind = kind;
  gGui.commandStatus = std::move(status);
  gGui.commandOutput.clear();
  gGui.pendingCommand = std::async(
      std::launch::async, [command = std::move(command)] { return command(); });
}

static void UpdatePendingCommand(HALSimXRP& simXRP) {
  if (!gGui.pendingCommand.valid() ||
      gGui.pendingCommand.wait_for(0s) != std::future_status::ready) {
    return;
  }

  CommandResult result = gGui.pendingCommand.get();
  gGui.commandOutput = std::move(result.output);
  if (gGui.pendingKind == CommandKind::REFRESH ||
      gGui.pendingKind == CommandKind::SCAN) {
    gGui.devices = std::move(result.devices);
    gGui.selectedDevice =
        gGui.devices.empty()
            ? -1
            : std::clamp(gGui.selectedDevice, 0,
                         static_cast<int>(gGui.devices.size()) - 1);
  }

  if (result.exitCode == 0) {
    gGui.commandStatus = "Ready";
    if (result.rememberTarget) {
      simXRP.RememberBluetoothTarget(std::move(result.targetAddress),
                                     result.addressType);
    }
  } else if (!gGui.commandOutput.empty()) {
    gGui.commandStatus = "Command finished with output";
  } else {
    gGui.commandStatus = "Command failed";
  }
  gGui.pendingKind = CommandKind::NONE;
}

static void InitializeFromConnection(const XRPConnectionStatus& status) {
  if (gGui.initializedFromConnection) {
    return;
  }

  SetAddress(status.targetAddress);
  gGui.addressType =
      status.addressType == XRPBluetoothAddressType::PUBLIC ? 0 : 1;
  gGui.initializedFromConnection = true;
}

static std::string SelectedDeviceLabel() {
  if (gGui.selectedDevice < 0 ||
      gGui.selectedDevice >= static_cast<int>(gGui.devices.size())) {
    return "Select device";
  }

  const auto& device = gGui.devices[gGui.selectedDevice];
  if (device.name.empty()) {
    return device.target;
  }
  return device.name + " (" + device.target + ")";
}

static void DrawDeviceControls(bool commandRunning) {
  ImGui::BeginDisabled(commandRunning);
  if (ImGui::Button("Refresh")) {
    StartCommand(CommandKind::REFRESH, "Refreshing devices", RefreshDevices);
  }
  ImGui::SameLine();
  if (ImGui::Button("Scan")) {
    StartCommand(CommandKind::SCAN, "Scanning for Bluetooth devices",
                 [] { return ScanDevices(8s); });
  }
  ImGui::EndDisabled();

  std::string selectedLabel = SelectedDeviceLabel();
  if (ImGui::BeginCombo("Device", selectedLabel.c_str())) {
    for (int i = 0; i < static_cast<int>(gGui.devices.size()); ++i) {
      const auto& device = gGui.devices[i];
      std::string label = device.name.empty()
                              ? device.target
                              : device.name + " (" + device.target + ")";
      if (ImGui::Selectable(label.c_str(), gGui.selectedDevice == i)) {
        gGui.selectedDevice = i;
        SetAddress(device.target);
        gGui.addressType =
            device.addressType == XRPBluetoothAddressType::PUBLIC ? 0 : 1;
      }
    }
    ImGui::EndCombo();
  }

  ImGui::InputText("Target", gGui.address, sizeof(gGui.address));
  const char* addressTypes[] = {"Public", "Random"};
  ImGui::Combo("Address type", &gGui.addressType, addressTypes, 2);
}

static void DrawConnectionControls(HALSimXRP& simXRP,
                                   const XRPConnectionStatus& status,
                                   bool commandRunning) {
  bool targetValid = gGui.address[0] != '\0';
  bool pairingSupported =
      wpi::net::BluetoothLEPacketClient::IsPairingSupported();
  if (pairingSupported) {
    ImGui::BeginDisabled(commandRunning || !targetValid);
    if (ImGui::Button("Pair")) {
      std::string target = gGui.address;
      XRPBluetoothAddressType addressType =
          gGui.addressType == 0 ? XRPBluetoothAddressType::PUBLIC
                                : XRPBluetoothAddressType::RANDOM;
      StartCommand(CommandKind::PAIR, "Pairing device", [target, addressType] {
        return PairDevice(target, addressType);
      });
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
  }
  ImGui::BeginDisabled(commandRunning || !targetValid);
  if (ImGui::Button("Connect")) {
    simXRP.ConnectBluetooth(
        gGui.address, gGui.addressType == 0 ? XRPBluetoothAddressType::PUBLIC
                                            : XRPBluetoothAddressType::RANDOM);
  }
  ImGui::EndDisabled();

  ImGui::SameLine();
  ImGui::BeginDisabled(!status.connected && !status.connecting);
  if (ImGui::Button("Disconnect")) {
    simXRP.DisconnectBluetooth();
  }
  ImGui::EndDisabled();
}

static void DrawGuiImpl() {
  auto simXRP = gSimXRP.lock();
  if (!simXRP) {
    return;
  }

  UpdatePendingCommand(*simXRP);

  auto status = simXRP->GetConnectionStatus();
  InitializeFromConnection(status);

  ImGui::SetNextWindowSize(ImVec2{430, 0}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("XRP Bluetooth")) {
    ImGui::End();
    return;
  }

  ImVec4 statusColor = status.connected    ? ImVec4{0.25f, 0.75f, 0.35f, 1.0f}
                       : status.connecting ? ImVec4{0.95f, 0.7f, 0.25f, 1.0f}
                                           : ImVec4{0.75f, 0.75f, 0.75f, 1.0f};
  ImGui::TextColored(statusColor, "%s", status.status.c_str());
  if (!status.error.empty()) {
    ImGui::TextWrapped("%s", status.error.c_str());
  }

  ImGui::Separator();

  bool commandRunning = gGui.pendingCommand.valid();
  if (status.supported) {
    DrawDeviceControls(commandRunning);
    DrawConnectionControls(*simXRP, status, commandRunning);
  } else {
    ImGui::TextWrapped("%s", status.status.c_str());
  }

  if (!gGui.commandStatus.empty()) {
    ImGui::Separator();
    ImGui::TextUnformatted(gGui.commandStatus.c_str());
  }
  if (!gGui.commandOutput.empty()) {
    ImGui::TextWrapped("%s", gGui.commandOutput.c_str());
  }

  ImGui::Separator();
  ImGui::Text("Packets: sent %" PRIu64 ", received %" PRIu64,
              status.packetsSent, status.packetsReceived);

  ImGui::End();
}

static void DrawGui() {
  if (!gGetImguiContext) {
    return;
  }

  ImGuiContext* guiContext = gGetImguiContext();
  if (!guiContext) {
    return;
  }

  ImGuiContext* previousContext = ImGui::GetCurrentContext();
  ImGui::SetCurrentContext(guiContext);
  DrawGuiImpl();
  ImGui::SetCurrentContext(previousContext);
}

static void ExtensionListener(void*, const char* name, void* data) {
  std::string_view nameView{name};
  if (nameView == ADD_GUI_LATE_EXECUTE_NAME && !gLateExecuteRegistered) {
    auto addGuiLateExecute = reinterpret_cast<AddGuiLateExecuteFn>(data);
    addGuiLateExecute(DrawGui);
    gLateExecuteRegistered = true;
  } else if (nameView == GET_IMGUI_CONTEXT_NAME) {
    gGetImguiContext = reinterpret_cast<GetImguiContextFn>(data);
  }
}

}  // namespace

void wpilibxrp::InitializeXRPBluetoothGui(std::shared_ptr<HALSimXRP> simXRP) {
  gSimXRP = std::move(simXRP);
  if (!gListenerRegistered) {
    HAL_RegisterExtensionListener(nullptr, ExtensionListener);
    gListenerRegistered = true;
  }
}
