// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/HALSimXRPGui.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cinttypes>
#include <cstdio>
#include <cstring>
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

using namespace wpilibxrp;
using namespace std::chrono_literals;

namespace {

constexpr std::string_view ADD_GUI_LATE_EXECUTE_NAME =
    "halsimgui::AddGuiLateExecute";
constexpr std::string_view GET_IMGUI_CONTEXT_NAME =
    "halsimgui::GetImguiContext";

using AddGuiLateExecuteFn = void (*)(std::function<void()> execute);
using GetImguiContextFn = ImGuiContext* (*)();

struct BluetoothDevice {
  std::string address;
  std::string name;
};

struct CommandResult {
  int exitCode = -1;
  std::string output;
  std::vector<BluetoothDevice> devices;
};

enum class CommandKind { NONE, REFRESH, SCAN, PAIR };

struct GuiState {
  bool initializedFromConnection = false;
  char address[128] = "";
  int addressType = 1;
  int selectedDevice = -1;
  std::vector<BluetoothDevice> devices;
  std::future<CommandResult> pendingCommand;
  CommandKind pendingKind = CommandKind::NONE;
  std::string commandStatus;
  std::string commandOutput;
};

std::weak_ptr<HALSimXRP> gSimXRP;
GetImguiContextFn gGetImguiContext = nullptr;
bool gListenerRegistered = false;
bool gLateExecuteRegistered = false;
GuiState gGui;

int HexDigit(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  }
  if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  }
  return -1;
}

bool IsBluetoothAddress(std::string_view address) {
  if (address.size() != 17) {
    return false;
  }

  for (int i = 0; i < 6; ++i) {
    size_t pos = static_cast<size_t>(i * 3);
    if (HexDigit(address[pos]) < 0 || HexDigit(address[pos + 1]) < 0) {
      return false;
    }
    if (i < 5 && address[pos + 2] != ':') {
      return false;
    }
  }

  return true;
}

void SetAddress(std::string_view address) {
  std::snprintf(gGui.address, sizeof(gGui.address), "%.*s",
                static_cast<int>(address.size()), address.data());
}

std::vector<BluetoothDevice> ParseDevices(std::string_view output) {
  std::vector<BluetoothDevice> devices;
  size_t pos = 0;
  while (pos < output.size()) {
    size_t end = output.find('\n', pos);
    if (end == std::string_view::npos) {
      end = output.size();
    }
    std::string_view line = output.substr(pos, end - pos);
    pos = end + 1;

    size_t devicePos = line.find("Device ");
    if (devicePos == std::string_view::npos) {
      continue;
    }

    size_t addressPos = devicePos + 7;
    if (addressPos + 17 > line.size()) {
      continue;
    }

    std::string address{line.substr(addressPos, 17)};
    if (!IsBluetoothAddress(address)) {
      continue;
    }

    std::string name;
    if (addressPos + 18 < line.size()) {
      name = std::string{line.substr(addressPos + 18)};
    }

    devices.emplace_back(std::move(address), std::move(name));
  }

  std::stable_sort(devices.begin(), devices.end(),
                   [](const BluetoothDevice& a, const BluetoothDevice& b) {
                     bool aIsXRP = a.name.rfind("WPIXRP-", 0) == 0;
                     bool bIsXRP = b.name.rfind("WPIXRP-", 0) == 0;
                     if (aIsXRP != bIsXRP) {
                       return aIsXRP;
                     }
                     return a.name < b.name;
                   });
  return devices;
}

CommandResult RunCommand(std::string command) {
  CommandResult result;
#ifdef __linux__
  command.append(" 2>&1");
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    result.output = "Failed to launch bluetoothctl";
    return result;
  }

  std::array<char, 256> buffer{};
  while (std::fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result.output.append(buffer.data());
  }

  result.exitCode = pclose(pipe);
#else
  result.output =
      "Bluetooth pairing commands are only available on Linux. Enter the "
      "platform Bluetooth identifier or advertised XRP name to connect.";
#endif
  return result;
}

CommandResult RefreshDevices() {
  CommandResult result = RunCommand("bluetoothctl devices");
  result.devices = ParseDevices(result.output);
  return result;
}

void StartCommand(CommandKind kind, std::string status,
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

void UpdatePendingCommand() {
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
  } else if (!gGui.commandOutput.empty()) {
    gGui.commandStatus = "Command finished with output";
  } else {
    gGui.commandStatus = "Command failed";
  }
  gGui.pendingKind = CommandKind::NONE;
}

void InitializeFromConnection(const XRPConnectionStatus& status) {
  if (gGui.initializedFromConnection) {
    return;
  }

  SetAddress(status.targetAddress);
  gGui.addressType =
      status.addressType == XRPBluetoothAddressType::PUBLIC ? 0 : 1;
  gGui.initializedFromConnection = true;
}

std::string SelectedDeviceLabel() {
  if (gGui.selectedDevice < 0 ||
      gGui.selectedDevice >= static_cast<int>(gGui.devices.size())) {
    return "Select device";
  }

  const auto& device = gGui.devices[gGui.selectedDevice];
  if (device.name.empty()) {
    return device.address;
  }
  return device.name + " (" + device.address + ")";
}

void DrawDeviceControls(bool commandRunning) {
  ImGui::BeginDisabled(commandRunning);
  if (ImGui::Button("Refresh")) {
    StartCommand(CommandKind::REFRESH, "Refreshing devices", RefreshDevices);
  }
  ImGui::SameLine();
  if (ImGui::Button("Scan")) {
    StartCommand(CommandKind::SCAN, "Scanning for Bluetooth devices", [] {
      RunCommand("bluetoothctl --timeout 8 scan on");
      return RefreshDevices();
    });
  }
  ImGui::EndDisabled();

  std::string selectedLabel = SelectedDeviceLabel();
  if (ImGui::BeginCombo("Device", selectedLabel.c_str())) {
    for (int i = 0; i < static_cast<int>(gGui.devices.size()); ++i) {
      const auto& device = gGui.devices[i];
      std::string label = device.name.empty()
                              ? device.address
                              : device.name + " (" + device.address + ")";
      if (ImGui::Selectable(label.c_str(), gGui.selectedDevice == i)) {
        gGui.selectedDevice = i;
        SetAddress(device.address);
      }
    }
    ImGui::EndCombo();
  }

  ImGui::InputText("Address / identifier", gGui.address, sizeof(gGui.address));
  const char* addressTypes[] = {"Public", "Random"};
  ImGui::Combo("Address type", &gGui.addressType, addressTypes, 2);
}

void DrawConnectionControls(HALSimXRP& simXRP,
                            const XRPConnectionStatus& status,
                            bool commandRunning) {
  bool addressValid = IsBluetoothAddress(gGui.address);
  bool targetValid = gGui.address[0] != '\0';
  ImGui::BeginDisabled(commandRunning || !addressValid);
  if (ImGui::Button("Pair & Trust")) {
    std::string address = gGui.address;
    StartCommand(CommandKind::PAIR, "Pairing device", [address] {
      return RunCommand("bluetoothctl pair " + address +
                        " && bluetoothctl trust " + address);
    });
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
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

void DrawGuiImpl() {
  auto simXRP = gSimXRP.lock();
  if (!simXRP) {
    return;
  }

  UpdatePendingCommand();

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

void DrawGui() {
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

void ExtensionListener(void*, const char* name, void* data) {
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
