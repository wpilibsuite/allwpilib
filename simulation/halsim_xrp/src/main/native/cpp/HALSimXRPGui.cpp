// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/HALSimXRPGui.hpp"

#include <algorithm>
#include <array>
#include <cfloat>
#include <chrono>
#include <cinttypes>
#include <cstdio>
#include <format>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>

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
constexpr std::string_view GET_IMPLOT_CONTEXT_NAME =
    "halsimgui::GetImPlotContext";
constexpr std::string_view XRP_DEVICE_NAME_PREFIX = "WPIXRP-";
constexpr double LATENCY_HISTORY_SECONDS = 10.0;
constexpr size_t LATENCY_MAX_SAMPLES = 1500;
constexpr float LATENCY_PLOT_HEIGHT = 110.0f;
constexpr double DATA_FADE_DELAY_SECONDS = 0.25;
constexpr double DATA_FADE_DURATION_SECONDS = 5.0;
constexpr float DATA_STALE_TEXT_BRIGHTNESS = 0.35f;

constexpr std::array<std::string_view, 4> MOTOR_LABELS = {
    "Left motor", "Right motor", "Motor 3", "Motor 4"};
constexpr std::array<std::string_view, 4> SERVO_LABELS = {"Servo 1", "Servo 2",
                                                          "Servo 3", "Servo 4"};

using AddGuiLateExecuteFn = void (*)(std::function<void()> execute);
using GetImguiContextFn = ImGuiContext* (*)();
using GetImPlotContextFn = ImPlotContext* (*)();

struct CommandResult {
  int exitCode = -1;
  std::string output;
  std::vector<wpi::net::BluetoothLEDeviceInfo> devices;
  std::string targetAddress;
  std::string targetName;
  XRPBluetoothAddressType addressType = XRPBluetoothAddressType::RANDOM;
  bool rememberTarget = false;
};

enum class CommandKind { NONE, SCAN, PAIR };

struct GuiState {
  bool initializedFromConnection = false;
  char address[128] = "";
  int addressType = 1;
  int selectedDevice = -1;
  std::vector<wpi::net::BluetoothLEDeviceInfo> devices;
  std::future<CommandResult> pendingCommand;
  CommandKind pendingKind = CommandKind::NONE;
  std::string scanStatus;
  std::string commandStatus;
  std::string commandOutput;
  std::vector<double> latencyTimes;
  std::vector<double> roundTripLatencyMs;
  std::vector<double> xrpControlRxAgeMs;
  std::string latencyTarget;
  uint16_t lastLatencyControlSeq = 0;
  bool haveLastLatencyControlSeq = false;
  bool showAddress = false;
};

static std::weak_ptr<HALSimXRP> gSimXRP;
static GetImguiContextFn gGetImguiContext = nullptr;
static GetImPlotContextFn gGetImPlotContext = nullptr;
static bool gListenerRegistered = false;
static bool gLateExecuteRegistered = false;
static GuiState gGui;

static void SetAddress(std::string_view address) {
  std::snprintf(gGui.address, sizeof(gGui.address), "%.*s",
                static_cast<int>(address.size()), address.data());
}

static XRPBluetoothAddressType GetGuiAddressType() {
  return gGui.addressType == 0 ? XRPBluetoothAddressType::PUBLIC
                               : XRPBluetoothAddressType::RANDOM;
}

static bool IsXRPDevice(const wpi::net::BluetoothLEDeviceInfo& device) {
  return device.name.rfind(XRP_DEVICE_NAME_PREFIX, 0) == 0;
}

static std::string_view GetDeviceDisplayName(std::string_view name) {
  if (name.rfind(XRP_DEVICE_NAME_PREFIX, 0) == 0) {
    return name.substr(XRP_DEVICE_NAME_PREFIX.size());
  }
  return name;
}

static std::string_view GetDeviceSortKey(
    const wpi::net::BluetoothLEDeviceInfo& device) {
  std::string_view displayName = GetDeviceDisplayName(device.name);
  return displayName.empty() ? std::string_view{device.target} : displayName;
}

static std::string GetDeviceLabel(const wpi::net::BluetoothLEDeviceInfo& device,
                                  bool unique) {
  std::string_view displayName = GetDeviceDisplayName(device.name);
  std::string label;
  if (displayName.empty()) {
    label = gGui.showAddress ? device.target : "XRP device";
  } else if (gGui.showAddress) {
    label = std::string{displayName} + " (" + device.target + ")";
  } else {
    label = std::string{displayName};
  }

  if (unique) {
    label += "###";
    label += device.target;
    label += device.addressType == XRPBluetoothAddressType::PUBLIC ? "#PUBLIC"
                                                                   : "#RANDOM";
  }
  return label;
}

static bool HamburgerButton(const ImGuiID id, const ImVec2 position) {
  const ImGuiStyle& style = ImGui::GetStyle();
  ImGuiWindow* window = ImGui::GetCurrentWindow();

  const ImRect bb{
      position, position + ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()) +
                    style.FramePadding * 2.0f};

  ImGui::ItemAdd(bb, id);

  bool hovered;
  bool held;
  bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

  const ImU32 bgCol =
      ImGui::GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
  const ImVec2 center = bb.GetCenter();
  if (hovered) {
    window->DrawList->AddCircleFilled(
        center, ImMax(2.0f, ImGui::GetFontSize() * 0.5f + 1.0f), bgCol, 12);
  }

  const ImU32 fgCol = ImGui::GetColorU32(ImGuiCol_Text);
  const float halfLineWidth = ImGui::GetFontSize() * 0.5f * 0.7071f;
  const float halfTotalHeight = halfLineWidth * 0.875f;
  ImVec2 lineStart = {center.x - halfLineWidth, center.y - halfTotalHeight};
  ImVec2 lineEnd = {center.x + halfLineWidth, center.y - halfTotalHeight};
  ImVec2 increment = {0.0f, halfTotalHeight};

  for (int i = 0; i < 3; ++i) {
    window->DrawList->AddLine(lineStart, lineEnd, fgCol);
    lineStart += increment;
    lineEnd += increment;
  }

  return pressed;
}

static void DrawViewSettingsMenu() {
  bool titleBarClicked =
      ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered();
  ImGuiWindow* window = ImGui::GetCurrentWindow();

  bool settingsButtonClicked = false;
  if (!ImGui::IsWindowDocked() &&
      ImGui::GetWindowWidth() > (ImGui::GetFontSize() + 2) * 3 +
                                    ImGui::GetStyle().FramePadding.x * 2) {
    const ImGuiItemFlags itemFlagsRestore =
        ImGui::GetCurrentContext()->CurrentItemFlags;

    ImGui::GetCurrentContext()->CurrentItemFlags |=
        ImGuiItemFlags_NoNavDefaultFocus;
    window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;

    ImGui::PushClipRect(window->OuterRectClipped.Min,
                        window->OuterRectClipped.Max, false);

    const ImRect titleBarRect = window->TitleBarRect();
    const ImVec2 position = {titleBarRect.Max.x -
                                 (ImGui::GetStyle().FramePadding.x * 3) -
                                 (ImGui::GetFontSize() * 2),
                             titleBarRect.Min.y};
    settingsButtonClicked =
        HamburgerButton(ImGui::GetID("#VIEW_SETTINGS"), position);

    ImGui::PopClipRect();
    ImGui::GetCurrentContext()->CurrentItemFlags = itemFlagsRestore;
  }

  if (settingsButtonClicked || titleBarClicked) {
    ImGui::OpenPopup("View Settings");
  }

  if (ImGui::BeginPopup("View Settings",
                        ImGuiWindowFlags_AlwaysAutoResize |
                            ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoSavedSettings)) {
    ImGui::TextUnformatted("View Settings");
    ImGui::Separator();
    ImGui::Checkbox("Show address", &gGui.showAddress);
    ImGui::EndPopup();
  }
}

static int FindDevice(std::string_view target,
                      XRPBluetoothAddressType addressType) {
  auto device =
      std::find_if(gGui.devices.begin(), gGui.devices.end(),
                   [=](const wpi::net::BluetoothLEDeviceInfo& candidate) {
                     return candidate.target == target &&
                            candidate.addressType == addressType;
                   });
  if (device == gGui.devices.end()) {
    return -1;
  }
  return static_cast<int>(device - gGui.devices.begin());
}

static void SelectDevice(int deviceIndex) {
  gGui.selectedDevice = deviceIndex;
  if (deviceIndex < 0 || deviceIndex >= static_cast<int>(gGui.devices.size())) {
    return;
  }

  const auto& device = gGui.devices[deviceIndex];
  SetAddress(device.target);
  gGui.addressType =
      device.addressType == XRPBluetoothAddressType::PUBLIC ? 0 : 1;
}

static void UpsertDevice(std::string_view target,
                         XRPBluetoothAddressType addressType,
                         std::string_view name) {
  if (target.empty()) {
    return;
  }

  int deviceIndex = FindDevice(target, addressType);
  if (deviceIndex < 0) {
    wpi::net::BluetoothLEDeviceInfo device;
    device.target = target;
    device.name = name;
    device.addressType = addressType;
    gGui.devices.emplace_back(std::move(device));
    SelectDevice(static_cast<int>(gGui.devices.size()) - 1);
    return;
  }

  auto& device = gGui.devices[deviceIndex];
  if (!name.empty()) {
    device.name = name;
  }
  SelectDevice(deviceIndex);
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

static std::string FormatXRPDeviceCount(size_t count) {
  return std::format("{} XRP {}", count, count == 1 ? "device" : "devices");
}

static CommandResult ScanDevices(std::chrono::milliseconds timeout) {
  CommandResult result;
  auto scan = wpi::net::BluetoothLEPacketClient::ScanDevices(timeout);
  result.devices = std::move(scan.devices);
  FilterAndSortXRPDevices(&result.devices);
  result.exitCode = scan.error.empty() ? 0 : 1;
  result.output = scan.error.empty()
                      ? FormatXRPDeviceCount(result.devices.size())
                      : std::move(scan.error);
  return result;
}

static CommandResult PairDevice(std::string_view target,
                                XRPBluetoothAddressType addressType,
                                std::string_view name) {
  CommandResult result;
  auto pairing = wpi::net::BluetoothLEPacketClient::PairDevice(target);
  result.exitCode = pairing.paired ? 0 : 1;
  result.output = pairing.error.empty() ? std::move(pairing.status)
                                        : std::move(pairing.error);
  if (pairing.paired) {
    result.targetAddress = target;
    result.targetName = name;
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
  if (kind == CommandKind::SCAN) {
    gGui.scanStatus = std::move(status);
    gGui.commandStatus.clear();
  } else {
    gGui.commandStatus = std::move(status);
  }
  gGui.commandOutput.clear();
  gGui.pendingCommand = std::async(
      std::launch::async, [command = std::move(command)] { return command(); });
}

static void UpdatePendingCommand(HALSimXRP& simXRP) {
  if (!gGui.pendingCommand.valid() ||
      gGui.pendingCommand.wait_for(0s) != std::future_status::ready) {
    return;
  }

  CommandKind commandKind = gGui.pendingKind;
  CommandResult result = gGui.pendingCommand.get();
  gGui.commandOutput.clear();
  if (commandKind == CommandKind::SCAN) {
    gGui.devices = std::move(result.devices);
    int currentDevice = FindDevice(gGui.address, GetGuiAddressType());
    int selectedDevice =
        currentDevice >= 0
            ? currentDevice
            : (gGui.devices.empty()
                   ? -1
                   : std::clamp(gGui.selectedDevice, 0,
                                static_cast<int>(gGui.devices.size()) - 1));
    SelectDevice(selectedDevice);
  }

  if (result.exitCode == 0) {
    if (commandKind == CommandKind::SCAN) {
      gGui.scanStatus = std::move(result.output);
      gGui.commandStatus.clear();
    } else {
      gGui.commandStatus = "Ready";
      gGui.commandOutput = std::move(result.output);
    }
    if (result.rememberTarget) {
      simXRP.RememberBluetoothTarget(std::move(result.targetAddress),
                                     result.addressType,
                                     std::move(result.targetName));
    }
  } else {
    if (commandKind == CommandKind::SCAN) {
      gGui.scanStatus = "Scan failed";
      gGui.commandStatus.clear();
    } else if (!result.output.empty()) {
      gGui.commandStatus = "Command finished with output";
    } else {
      gGui.commandStatus = "Command failed";
    }
    gGui.commandOutput = std::move(result.output);
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
  UpsertDevice(status.targetAddress, status.addressType, status.targetName);
  gGui.initializedFromConnection = true;
}

static void ClearLatencyHistory() {
  gGui.latencyTimes.clear();
  gGui.roundTripLatencyMs.clear();
  gGui.xrpControlRxAgeMs.clear();
  gGui.latencyTarget.clear();
  gGui.haveLastLatencyControlSeq = false;
}

static void PruneLatencyHistory(double minimumTime) {
  auto firstVisible = std::lower_bound(gGui.latencyTimes.begin(),
                                       gGui.latencyTimes.end(), minimumTime);
  auto eraseCount =
      static_cast<size_t>(firstVisible - gGui.latencyTimes.begin());

  if (gGui.latencyTimes.size() - eraseCount > LATENCY_MAX_SAMPLES) {
    eraseCount = gGui.latencyTimes.size() - LATENCY_MAX_SAMPLES;
  }

  if (eraseCount == 0) {
    return;
  }

  gGui.latencyTimes.erase(gGui.latencyTimes.begin(),
                          gGui.latencyTimes.begin() + eraseCount);
  gGui.roundTripLatencyMs.erase(gGui.roundTripLatencyMs.begin(),
                                gGui.roundTripLatencyMs.begin() + eraseCount);
  gGui.xrpControlRxAgeMs.erase(gGui.xrpControlRxAgeMs.begin(),
                               gGui.xrpControlRxAgeMs.begin() + eraseCount);
}

static void UpdateLatencyHistory(const XRPConnectionStatus& status) {
  if (!status.connected) {
    ClearLatencyHistory();
    return;
  }

  if (gGui.latencyTarget != status.targetAddress) {
    ClearLatencyHistory();
    gGui.latencyTarget = status.targetAddress;
  }

  if (!status.latencyAvailable ||
      (gGui.haveLastLatencyControlSeq &&
       gGui.lastLatencyControlSeq == status.latencyControlSeq)) {
    return;
  }

  double now = ImGui::GetTime();
  gGui.latencyTimes.emplace_back(now);
  gGui.roundTripLatencyMs.emplace_back(status.roundTripLatencyMs);
  gGui.xrpControlRxAgeMs.emplace_back(status.xrpControlRxAgeMs);
  gGui.lastLatencyControlSeq = status.latencyControlSeq;
  gGui.haveLastLatencyControlSeq = true;

  PruneLatencyHistory(now - LATENCY_HISTORY_SECONDS);
}

static void DrawLatencyPlot(const XRPConnectionStatus& status) {
  UpdateLatencyHistory(status);
  if (!status.connected) {
    return;
  }

  ImGui::TextUnformatted("Comms latency");
  if (ImPlot::GetCurrentContext() == nullptr) {
    ImGui::TextUnformatted("Latency plot unavailable");
    return;
  }
  if (gGui.latencyTimes.empty()) {
    ImGui::TextUnformatted("Waiting for timing data");
    return;
  }

  double now = gGui.latencyTimes.back();
  double minimumTime = std::max(0.0, now - LATENCY_HISTORY_SECONDS);
  double maximumLatencyMs = 5.0;
  for (size_t i = 0; i < gGui.latencyTimes.size(); ++i) {
    if (gGui.latencyTimes[i] < minimumTime) {
      continue;
    }
    maximumLatencyMs = std::max(maximumLatencyMs, gGui.roundTripLatencyMs[i]);
    maximumLatencyMs = std::max(maximumLatencyMs, gGui.xrpControlRxAgeMs[i]);
  }
  maximumLatencyMs *= 1.25;

  constexpr ImPlotFlags PLOT_FLAGS =
      ImPlotFlags_NoTitle | ImPlotFlags_NoMouseText | ImPlotFlags_NoMenus |
      ImPlotFlags_NoBoxSelect;
  constexpr ImPlotAxisFlags TIME_AXIS_FLAGS =
      ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoGridLines |
      ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels |
      ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight |
      ImPlotAxisFlags_Lock;
  constexpr ImPlotAxisFlags LATENCY_AXIS_FLAGS =
      ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoMenus |
      ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_LockMin;

  if (ImPlot::BeginPlot("##XRPCommsLatency", ImVec2{-1, LATENCY_PLOT_HEIGHT},
                        PLOT_FLAGS)) {
    ImPlot::SetupLegend(
        ImPlotLocation_NorthWest,
        ImPlotLegendFlags_Horizontal | ImPlotLegendFlags_NoMenus);
    ImPlot::SetupAxis(ImAxis_X1, nullptr, TIME_AXIS_FLAGS);
    ImPlot::SetupAxisLimits(ImAxis_X1, minimumTime, now, ImGuiCond_Always);
    ImPlot::SetupAxis(ImAxis_Y1, nullptr, LATENCY_AXIS_FLAGS);
    ImPlot::SetupAxisFormat(ImAxis_Y1, "%.0f ms");
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, maximumLatencyMs, ImGuiCond_Always);
    ImPlot::SetupFinish();

    int sampleCount = static_cast<int>(gGui.latencyTimes.size());
    ImPlot::PlotLine("RTT", gGui.latencyTimes.data(),
                     gGui.roundTripLatencyMs.data(), sampleCount);
    ImPlot::PlotLine("XRP age", gGui.latencyTimes.data(),
                     gGui.xrpControlRxAgeMs.data(), sampleCount);
    ImPlot::EndPlot();
  }
}

static double DataAgeSeconds(std::chrono::steady_clock::time_point lastUpdate) {
  return std::chrono::duration<double>(std::chrono::steady_clock::now() -
                                       lastUpdate)
      .count();
}

static ImVec4 GetDataTextColor(
    bool present, std::chrono::steady_clock::time_point lastUpdate) {
  if (!present) {
    return {DATA_STALE_TEXT_BRIGHTNESS, DATA_STALE_TEXT_BRIGHTNESS,
            DATA_STALE_TEXT_BRIGHTNESS, 1.0f};
  }

  double age = DataAgeSeconds(lastUpdate);
  float fade = static_cast<float>((age - DATA_FADE_DELAY_SECONDS) /
                                  DATA_FADE_DURATION_SECONDS);
  fade = std::clamp(fade, 0.0f, 1.0f);
  float brightness = 1.0f - (1.0f - DATA_STALE_TEXT_BRIGHTNESS) * fade;
  return {brightness, brightness, brightness, 1.0f};
}

static void TextUnformatted(std::string_view text) {
  ImGui::TextUnformatted(text.data(), text.data() + text.size());
}

template <typename Formatter>
static void DrawDataRow(std::string_view name, bool present,
                        std::chrono::steady_clock::time_point lastUpdate,
                        Formatter formatter) {
  ImGui::TableNextRow();
  ImGui::PushStyleColor(ImGuiCol_Text, GetDataTextColor(present, lastUpdate));
  ImGui::TableNextColumn();
  TextUnformatted(name);
  ImGui::TableNextColumn();
  std::string value = present ? formatter() : "--";
  ImGui::TextUnformatted(value.c_str());
  ImGui::PopStyleColor();
}

static std::string FormatBool(bool value, std::string_view trueText,
                              std::string_view falseText) {
  return std::string{value ? trueText : falseText};
}

static std::string FormatSigned3(double value) {
  return std::format("{:+.3f}", value);
}

static std::string FormatSigned4(double value) {
  return std::format("{:+.4f}", value);
}

static std::string FormatMotorOutput(float value) {
  return FormatSigned3(value);
}

static std::string FormatServoOutput(float value) {
  return std::format("{:.1f} deg ({:.1f}%)", value * SERVO_MAX_DEGREES,
                     value * 100.0f);
}

static std::string FormatDigitalValue(bool value) {
  return value ? "High" : "Low";
}

static std::string FormatEncoderData(const XRPEncoderData& value) {
  if (!value.periodValid) {
    return std::format("count {}, period --", value.count);
  }
  return std::format("count {}, period {} s", value.count,
                     FormatSigned4(value.period));
}

static bool BeginXRPDataTable(const char* id) {
  constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_Borders |
                                          ImGuiTableFlags_RowBg |
                                          ImGuiTableFlags_SizingStretchProp;
  if (!ImGui::BeginTable(id, 2, TABLE_FLAGS)) {
    return false;
  }

  float fontSize = ImGui::GetFontSize();
  ImGui::TableSetupColumn("Signal", ImGuiTableColumnFlags_WidthFixed,
                          fontSize * 9.0f);
  ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
  ImGui::TableHeadersRow();
  return true;
}

static void DrawControlDataTable(const XRPControlData& control) {
  if (!BeginXRPDataTable("XRPControlDataTable")) {
    return;
  }

  DrawDataRow(
      "Robot state", control.packet.present, control.packet.lastUpdate,
      [&] { return FormatBool(control.enabled, "Enabled", "Disabled"); });

  for (size_t i = 0; i < control.motors.size(); ++i) {
    const auto& motor = control.motors[i];
    DrawDataRow(MOTOR_LABELS[i], motor.present, motor.lastUpdate,
                [&] { return FormatMotorOutput(motor.value); });
  }

  for (size_t i = 0; i < control.servos.size(); ++i) {
    const auto& servo = control.servos[i];
    DrawDataRow(SERVO_LABELS[i], servo.present, servo.lastUpdate,
                [&] { return FormatServoOutput(servo.value); });
  }

  for (size_t i = 0; i < control.digitalOutputs.size(); ++i) {
    const auto& dio = control.digitalOutputs[i];
    std::string label = std::format("DIO {} output", i);
    DrawDataRow(label, dio.present, dio.lastUpdate,
                [&] { return FormatDigitalValue(dio.value); });
  }

  ImGui::EndTable();
}

static void DrawStatusDataTable(const XRPStatusData& status) {
  if (!BeginXRPDataTable("XRPStatusDataTable")) {
    return;
  }

  for (size_t i = 0; i < status.encoders.size(); ++i) {
    const auto& encoder = status.encoders[i];
    std::string label = std::format("Encoder {}", i);
    DrawDataRow(label, encoder.present, encoder.lastUpdate,
                [&] { return FormatEncoderData(encoder.value); });
  }

  for (size_t i = 0; i < status.digitalInputs.size(); ++i) {
    const auto& dio = status.digitalInputs[i];
    std::string label = std::format("DIO {} input", i);
    DrawDataRow(label, dio.present, dio.lastUpdate,
                [&] { return FormatDigitalValue(dio.value); });
  }

  DrawDataRow("Gyro rate X", status.gyro.present, status.gyro.lastUpdate, [&] {
    return std::format("{} deg/s", FormatSigned3(status.gyro.value.rate.x));
  });
  DrawDataRow("Gyro rate Y", status.gyro.present, status.gyro.lastUpdate, [&] {
    return std::format("{} deg/s", FormatSigned3(status.gyro.value.rate.y));
  });
  DrawDataRow("Gyro rate Z", status.gyro.present, status.gyro.lastUpdate, [&] {
    return std::format("{} deg/s", FormatSigned3(status.gyro.value.rate.z));
  });
  DrawDataRow("Gyro angle X", status.gyro.present, status.gyro.lastUpdate, [&] {
    return std::format("{} deg", FormatSigned3(status.gyro.value.angle.x));
  });
  DrawDataRow("Gyro angle Y", status.gyro.present, status.gyro.lastUpdate, [&] {
    return std::format("{} deg", FormatSigned3(status.gyro.value.angle.y));
  });
  DrawDataRow("Gyro angle Z", status.gyro.present, status.gyro.lastUpdate, [&] {
    return std::format("{} deg", FormatSigned3(status.gyro.value.angle.z));
  });
  DrawDataRow("Accel X", status.accel.present, status.accel.lastUpdate,
              [&] { return FormatSigned3(status.accel.value.x); });
  DrawDataRow("Accel Y", status.accel.present, status.accel.lastUpdate,
              [&] { return FormatSigned3(status.accel.value.y); });
  DrawDataRow("Accel Z", status.accel.present, status.accel.lastUpdate,
              [&] { return FormatSigned3(status.accel.value.z); });

  for (size_t i = 0; i < status.analogInputs.size(); ++i) {
    const auto& analog = status.analogInputs[i];
    std::string label = std::format("Analog {}", i);
    DrawDataRow(label, analog.present, analog.lastUpdate,
                [&] { return std::format("{:.2f} V", analog.value); });
  }

  ImGui::EndTable();
}

static void DrawXRPDataWindow(const XRPDataSnapshot& data) {
  ImGui::SetNextWindowSize(ImVec2{620, 520}, ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("XRP Control and Status")) {
    ImGui::End();
    return;
  }

  ImGui::TextUnformatted("Control");
  DrawControlDataTable(data.control);
  ImGui::Separator();
  ImGui::TextUnformatted("Status");
  DrawStatusDataTable(data.status);

  ImGui::End();
}

static std::string SelectedDeviceLabel() {
  if (gGui.selectedDevice < 0 ||
      gGui.selectedDevice >= static_cast<int>(gGui.devices.size())) {
    return "Select device";
  }

  const auto& device = gGui.devices[gGui.selectedDevice];
  return GetDeviceLabel(device, false);
}

static std::string GetSelectedDeviceName(std::string_view target,
                                         XRPBluetoothAddressType addressType) {
  if (gGui.selectedDevice < 0 ||
      gGui.selectedDevice >= static_cast<int>(gGui.devices.size())) {
    return {};
  }

  const auto& device = gGui.devices[gGui.selectedDevice];
  if (device.target != target || device.addressType != addressType) {
    return {};
  }
  return std::string{GetDeviceDisplayName(device.name)};
}

static void DrawDeviceControls(bool commandRunning) {
  ImGui::BeginDisabled(commandRunning);
  if (ImGui::Button("Scan")) {
    StartCommand(CommandKind::SCAN, "Scanning...",
                 [] { return ScanDevices(8s); });
  }
  ImGui::EndDisabled();
  if (!gGui.scanStatus.empty()) {
    ImGui::SameLine();
    ImGui::TextDisabled("%s", gGui.scanStatus.c_str());
  }

  std::string selectedLabel = SelectedDeviceLabel();
  if (ImGui::BeginCombo("Device", selectedLabel.c_str())) {
    for (int i = 0; i < static_cast<int>(gGui.devices.size()); ++i) {
      const auto& device = gGui.devices[i];
      std::string label = GetDeviceLabel(device, true);
      if (ImGui::Selectable(label.c_str(), gGui.selectedDevice == i)) {
        SelectDevice(i);
      }
    }
    ImGui::EndCombo();
  }

  if (gGui.showAddress) {
    ImGui::InputText("Target", gGui.address, sizeof(gGui.address));
    const char* addressTypes[] = {"Public", "Random"};
    ImGui::Combo("Address type", &gGui.addressType, addressTypes, 2);
  }
}

static void DrawConnectionControls(HALSimXRP& simXRP,
                                   const XRPConnectionStatus& status,
                                   bool commandRunning) {
  bool targetValid = gGui.address[0] != '\0';
  bool connectionActive = status.connected || status.connecting;
  bool pairingSupported =
      wpi::net::BluetoothLEPacketClient::IsPairingSupported();
  if (pairingSupported && gGui.showAddress) {
    ImGui::BeginDisabled(commandRunning || !targetValid || connectionActive);
    if (ImGui::Button("Pair")) {
      std::string target = gGui.address;
      XRPBluetoothAddressType addressType = GetGuiAddressType();
      std::string name = GetSelectedDeviceName(target, addressType);
      StartCommand(CommandKind::PAIR, "Pairing device",
                   [target, addressType, name] {
                     return PairDevice(target, addressType, name);
                   });
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
  }
  ImGui::BeginDisabled(commandRunning || !targetValid || connectionActive);
  if (ImGui::Button("Connect")) {
    std::string target = gGui.address;
    XRPBluetoothAddressType addressType = GetGuiAddressType();
    simXRP.ConnectBluetooth(target, addressType,
                            GetSelectedDeviceName(target, addressType));
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
  XRPDataSnapshot data = simXRP->GetDataSnapshot();

  ImGui::SetNextWindowSize(ImVec2{430, 0}, ImGuiCond_FirstUseEver);
  float minWidth =
      ImGui::CalcTextSize("XRP Bluetooth").x + ImGui::GetFontSize() * 3 +
      ImGui::GetStyle().ItemInnerSpacing.x * 3 +
      ImGui::GetStyle().FramePadding.x * 3 + ImGui::GetStyle().WindowBorderSize;
  ImGui::SetNextWindowSizeConstraints({minWidth, 0}, ImVec2{FLT_MAX, FLT_MAX});
  if (!ImGui::Begin("XRP Bluetooth")) {
    ImGui::End();
    DrawXRPDataWindow(data);
    return;
  }
  DrawViewSettingsMenu();

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

  DrawLatencyPlot(status);

  ImGui::End();
  DrawXRPDataWindow(data);
}

static void DrawGui() {
  if (!gGetImguiContext) {
    return;
  }

  ImGuiContext* guiContext = gGetImguiContext();
  if (!guiContext) {
    return;
  }

  ImPlotContext* plotContext =
      gGetImPlotContext ? gGetImPlotContext() : nullptr;
  ImGuiContext* previousContext = ImGui::GetCurrentContext();
  ImPlotContext* previousPlotContext = ImPlot::GetCurrentContext();
  ImGui::SetCurrentContext(guiContext);
  ImPlot::SetCurrentContext(plotContext);
  DrawGuiImpl();
  ImPlot::SetCurrentContext(previousPlotContext);
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
  } else if (nameView == GET_IMPLOT_CONTEXT_NAME) {
    gGetImPlotContext = reinterpret_cast<GetImPlotContextFn>(data);
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
