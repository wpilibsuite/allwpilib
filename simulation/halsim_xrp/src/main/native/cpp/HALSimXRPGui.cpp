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

#include <IconsFontAwesome6Brands.h>
#include <imgui.h>
#include <imgui_FontAwesomeBrands.h>
#include <imgui_internal.h>
#include <implot.h>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/DataSource.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/glass/WindowManager.hpp"
#include "wpi/glass/support/ExtraGuiWidgets.hpp"
#include "wpi/hal/Extensions.h"
#include "wpi/halsim/xrp/HALSimXRP.hpp"
#include "wpi/net/BluetoothLEPacketClient.hpp"

using namespace wpilibxrp;
using namespace std::chrono_literals;

namespace {

constexpr std::string_view ADD_GUI_LATE_EXECUTE_NAME =
    "halsimgui::AddGuiLateExecute";
constexpr std::string_view ADD_MAIN_MENU_NAME = "halsimgui::AddMainMenu";
constexpr std::string_view GET_IMGUI_CONTEXT_NAME =
    "halsimgui::GetImguiContext";
constexpr std::string_view GET_IMPLOT_CONTEXT_NAME =
    "halsimgui::GetImPlotContext";
constexpr std::string_view GET_GLASS_CONTEXT_NAME =
    "halsimgui::GetGlassContext";
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
constexpr std::string_view XRP_BLUETOOTH_WINDOW_NAME = "XRP Bluetooth";
constexpr std::string_view XRP_CONTROL_WINDOW_NAME = "XRP Control";
constexpr std::string_view XRP_STATUS_WINDOW_NAME = "XRP Status";
constexpr ImWchar FONT_AWESOME_BLUETOOTH_CODEPOINT = 0xf294;

using AddGuiLateExecuteFn = void (*)(std::function<void()> execute);
using AddMainMenuFn = void (*)(std::function<void()> menu);
using GetImguiContextFn = ImGuiContext* (*)();
using GetImPlotContextFn = ImPlotContext* (*)();
using GetGlassContextFn = wpi::glass::Context* (*)();

class XRPWindowManager : public wpi::glass::WindowManager {
 public:
  using wpi::glass::WindowManager::WindowManager;

  void DisplayManagedWindows() {
    wpi::glass::PushStorageStack(m_storage);
    for (auto&& window : m_windows) {
      window->Display();
    }
    wpi::glass::PopStorageStack();
  }
};

template <typename Source>
struct SourceSlot {
  std::unique_ptr<Source> source;
  std::chrono::steady_clock::time_point lastUpdate;
  bool haveLastUpdate = false;
};

struct GuiDataSources {
  SourceSlot<wpi::glass::BooleanSource> robotEnabled;
  std::array<SourceSlot<wpi::glass::FloatSource>, 4> motors;
  std::array<SourceSlot<wpi::glass::FloatSource>, 4> servos;
  std::array<SourceSlot<wpi::glass::BooleanSource>, 8> digitalOutputs;
  std::array<SourceSlot<wpi::glass::IntegerSource>, 4> encoderCounts;
  std::array<SourceSlot<wpi::glass::FloatSource>, 4> encoderPeriods;
  std::array<SourceSlot<wpi::glass::BooleanSource>, 8> digitalInputs;
  std::array<SourceSlot<wpi::glass::FloatSource>, 3> gyroRates;
  std::array<SourceSlot<wpi::glass::FloatSource>, 3> gyroAngles;
  std::array<SourceSlot<wpi::glass::FloatSource>, 3> accelerometer;
  std::array<SourceSlot<wpi::glass::FloatSource>, 3> analogInputs;
  bool initialized = false;
};

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
  GuiDataSources dataSources;
  XRPDataSnapshot data;
  XRPConnectionStatus connectionStatus;
  ImFontAtlas* bluetoothIconFontAtlas = nullptr;
  ImFont* bluetoothIconFont = nullptr;
  std::unique_ptr<XRPWindowManager> windowManager;
};

static std::weak_ptr<HALSimXRP> gSimXRP;
static GetImguiContextFn gGetImguiContext = nullptr;
static GetImPlotContextFn gGetImPlotContext = nullptr;
static GetGlassContextFn gGetGlassContext = nullptr;
static bool gListenerRegistered = false;
static bool gLateExecuteRegistered = false;
static bool gMainMenuRegistered = false;
static GuiState gGui;

template <typename Func>
static void WithGuiContexts(Func&& func) {
  if (!gGetImguiContext) {
    return;
  }

  ImGuiContext* guiContext = gGetImguiContext();
  if (!guiContext) {
    return;
  }

  ImPlotContext* plotContext =
      gGetImPlotContext ? gGetImPlotContext() : nullptr;
  wpi::glass::Context* glassContext =
      gGetGlassContext ? gGetGlassContext() : nullptr;
  ImGuiContext* previousContext = ImGui::GetCurrentContext();
  ImPlotContext* previousPlotContext = ImPlot::GetCurrentContext();
  wpi::glass::Context* previousGlassContext = wpi::glass::GetCurrentContext();
  ImGui::SetCurrentContext(guiContext);
  ImPlot::SetCurrentContext(plotContext);
  wpi::glass::SetCurrentContext(glassContext);
  std::forward<Func>(func)();
  wpi::glass::SetCurrentContext(previousGlassContext);
  ImPlot::SetCurrentContext(previousPlotContext);
  ImGui::SetCurrentContext(previousContext);
}

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

  double now = ImGui::GetTime();
  double minimumTime = now - LATENCY_HISTORY_SECONDS;
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

static ImU32 GetDioLedColor(bool present,
                            std::chrono::steady_clock::time_point lastUpdate) {
  constexpr ImVec4 LED_COLOR = {1.0f, 1.0f, 0.4f, 1.0f};
  constexpr ImVec4 STALE_COLOR = {DATA_STALE_TEXT_BRIGHTNESS,
                                  DATA_STALE_TEXT_BRIGHTNESS,
                                  DATA_STALE_TEXT_BRIGHTNESS, 1.0f};
  if (!present) {
    return ImGui::ColorConvertFloat4ToU32(STALE_COLOR);
  }

  double age = DataAgeSeconds(lastUpdate);
  float fade = static_cast<float>((age - DATA_FADE_DELAY_SECONDS) /
                                  DATA_FADE_DURATION_SECONDS);
  fade = std::clamp(fade, 0.0f, 1.0f);
  ImVec4 color = {LED_COLOR.x + (STALE_COLOR.x - LED_COLOR.x) * fade,
                  LED_COLOR.y + (STALE_COLOR.y - LED_COLOR.y) * fade,
                  LED_COLOR.z + (STALE_COLOR.z - LED_COLOR.z) * fade, 1.0f};
  return ImGui::ColorConvertFloat4ToU32(color);
}

static void TextUnformatted(std::string_view text) {
  ImGui::TextUnformatted(text.data(), text.data() + text.size());
}

template <typename Source>
static void InitializeSource(SourceSlot<Source>& slot, std::string_view id,
                             std::string_view name) {
  slot.source = std::make_unique<Source>(std::string{id});
  slot.source->SetName(name);
}

static void InitializeDataSources() {
  if (gGui.dataSources.initialized ||
      wpi::glass::GetCurrentContext() == nullptr) {
    return;
  }

  auto& sources = gGui.dataSources;
  InitializeSource(sources.robotEnabled, "XRP/Control/RobotState",
                   "XRP Robot State");

  for (size_t i = 0; i < sources.motors.size(); ++i) {
    InitializeSource(sources.motors[i], std::format("XRP/Control/Motor/{}", i),
                     std::format("XRP {}", MOTOR_LABELS[i]));
  }

  for (size_t i = 0; i < sources.servos.size(); ++i) {
    InitializeSource(sources.servos[i],
                     std::format("XRP/Control/Servo/{}", i + 4),
                     std::format("XRP {}", SERVO_LABELS[i]));
  }

  for (size_t i = 0; i < sources.digitalOutputs.size(); ++i) {
    InitializeSource(sources.digitalOutputs[i],
                     std::format("XRP/Control/DIO/{}/Output", i),
                     std::format("XRP DIO {} Output", i));
  }

  for (size_t i = 0; i < sources.encoderCounts.size(); ++i) {
    InitializeSource(sources.encoderCounts[i],
                     std::format("XRP/Status/Encoder/{}/Count", i),
                     std::format("XRP Encoder {} Count", i));
    InitializeSource(sources.encoderPeriods[i],
                     std::format("XRP/Status/Encoder/{}/Period", i),
                     std::format("XRP Encoder {} Period", i));
  }

  for (size_t i = 0; i < sources.digitalInputs.size(); ++i) {
    InitializeSource(sources.digitalInputs[i],
                     std::format("XRP/Status/DIO/{}/Input", i),
                     std::format("XRP DIO {} Input", i));
  }

  constexpr std::array<std::string_view, 3> AXES = {"X", "Y", "Z"};
  for (size_t i = 0; i < AXES.size(); ++i) {
    InitializeSource(sources.gyroRates[i],
                     std::format("XRP/Status/Gyro/Rate/{}", AXES[i]),
                     std::format("XRP Gyro Rate {}", AXES[i]));
    InitializeSource(sources.gyroAngles[i],
                     std::format("XRP/Status/Gyro/Angle/{}", AXES[i]),
                     std::format("XRP Gyro Angle {}", AXES[i]));
    InitializeSource(sources.accelerometer[i],
                     std::format("XRP/Status/Accelerometer/{}", AXES[i]),
                     std::format("XRP Accelerometer {}", AXES[i]));
    InitializeSource(sources.analogInputs[i],
                     std::format("XRP/Status/Analog/{}", i),
                     std::format("XRP Analog {}", i));
  }

  sources.initialized = true;
}

template <typename Source, typename T>
static void UpdateSource(SourceSlot<Source>& slot, bool present,
                         std::chrono::steady_clock::time_point lastUpdate,
                         T value) {
  // UpdateDataSources() runs every GUI frame; only emit plot samples for new
  // XRP data.
  if (!slot.source || !present ||
      (slot.haveLastUpdate && slot.lastUpdate == lastUpdate)) {
    return;
  }

  slot.source->SetValue(value);
  slot.lastUpdate = lastUpdate;
  slot.haveLastUpdate = true;
}

static void UpdateDataSources(const XRPDataSnapshot& data) {
  InitializeDataSources();
  if (!gGui.dataSources.initialized) {
    return;
  }

  auto& sources = gGui.dataSources;
  UpdateSource(sources.robotEnabled, data.control.packet.present,
               data.control.packet.lastUpdate, data.control.enabled);

  for (size_t i = 0; i < data.control.motors.size(); ++i) {
    const auto& motor = data.control.motors[i];
    UpdateSource(sources.motors[i], motor.present, motor.lastUpdate,
                 motor.value);
  }

  for (size_t i = 0; i < data.control.servos.size(); ++i) {
    const auto& servo = data.control.servos[i];
    UpdateSource(sources.servos[i], servo.present, servo.lastUpdate,
                 servo.value * SERVO_MAX_DEGREES);
  }

  for (size_t i = 0; i < data.control.digitalOutputs.size(); ++i) {
    const auto& dio = data.control.digitalOutputs[i];
    UpdateSource(sources.digitalOutputs[i], dio.present, dio.lastUpdate,
                 dio.value);
  }

  for (size_t i = 0; i < data.status.encoders.size(); ++i) {
    const auto& encoder = data.status.encoders[i];
    UpdateSource(sources.encoderCounts[i], encoder.present, encoder.lastUpdate,
                 static_cast<int64_t>(encoder.value.count));
    UpdateSource(sources.encoderPeriods[i],
                 encoder.present && encoder.value.periodValid,
                 encoder.lastUpdate, static_cast<float>(encoder.value.period));
  }

  for (size_t i = 0; i < data.status.digitalInputs.size(); ++i) {
    const auto& dio = data.status.digitalInputs[i];
    UpdateSource(sources.digitalInputs[i], dio.present, dio.lastUpdate,
                 dio.value);
  }

  const auto& gyro = data.status.gyro;
  UpdateSource(sources.gyroRates[0], gyro.present, gyro.lastUpdate,
               gyro.value.rate.x);
  UpdateSource(sources.gyroRates[1], gyro.present, gyro.lastUpdate,
               gyro.value.rate.y);
  UpdateSource(sources.gyroRates[2], gyro.present, gyro.lastUpdate,
               gyro.value.rate.z);
  UpdateSource(sources.gyroAngles[0], gyro.present, gyro.lastUpdate,
               gyro.value.angle.x);
  UpdateSource(sources.gyroAngles[1], gyro.present, gyro.lastUpdate,
               gyro.value.angle.y);
  UpdateSource(sources.gyroAngles[2], gyro.present, gyro.lastUpdate,
               gyro.value.angle.z);

  const auto& accel = data.status.accel;
  UpdateSource(sources.accelerometer[0], accel.present, accel.lastUpdate,
               accel.value.x);
  UpdateSource(sources.accelerometer[1], accel.present, accel.lastUpdate,
               accel.value.y);
  UpdateSource(sources.accelerometer[2], accel.present, accel.lastUpdate,
               accel.value.z);

  for (size_t i = 0; i < data.status.analogInputs.size(); ++i) {
    const auto& analog = data.status.analogInputs[i];
    UpdateSource(sources.analogInputs[i], analog.present, analog.lastUpdate,
                 analog.value);
  }
}

struct DataRowComponent {
  bool present = false;
  std::chrono::steady_clock::time_point lastUpdate;
  const wpi::glass::DataSource* source = nullptr;
  std::string value;
};

template <typename Formatter>
static void DrawDataRow(std::string_view name, bool present,
                        std::chrono::steady_clock::time_point lastUpdate,
                        const wpi::glass::DataSource* source,
                        Formatter formatter) {
  ImGui::TableNextRow();
  ImGui::PushStyleColor(ImGuiCol_Text, GetDataTextColor(present, lastUpdate));
  ImGui::PushID(source);
  ImGui::TableNextColumn();
  if (source) {
    ImGui::Selectable(std::string{name}.c_str(), false);
    source->EmitDrag();
  } else {
    TextUnformatted(name);
  }
  ImGui::TableNextColumn();
  std::string value = present ? formatter() : "--";
  if (source) {
    ImGui::Selectable(std::format("{}##value", value).c_str(), false);
    source->EmitDrag();
  } else {
    ImGui::TextUnformatted(value.c_str());
  }
  ImGui::PopID();
  ImGui::PopStyleColor();
}

static float GetCompoundComponentWidth() {
  float componentWidth =
      ImGui::CalcTextSize("+").x + ImGui::CalcTextSize("0000.000").x;
  float suffixWidth = ImGui::CalcTextSize("deg/s").x;
  float spacing = ImGui::GetStyle().ItemSpacing.x * 2.0f;
  float availableWidth = ImGui::GetContentRegionAvail().x;
  float availableComponentWidth =
      (availableWidth - suffixWidth - ImGui::GetStyle().ItemSpacing.x -
       spacing * 2.0f) /
      3.0f;
  return std::max(ImGui::CalcTextSize("+").x + ImGui::CalcTextSize("--").x,
                  std::min(componentWidth, availableComponentWidth));
}

static void DrawDataRowComponent(const DataRowComponent& component,
                                 float width) {
  std::string display = component.present ? component.value : "--";
  std::string sign;
  std::string magnitude = display;
  if (!display.empty() && (display[0] == '+' || display[0] == '-')) {
    sign = display[0];
    magnitude = display.substr(1);
  }

  ImGui::PushStyleColor(
      ImGuiCol_Text, GetDataTextColor(component.present, component.lastUpdate));
  ImVec2 textPos = ImGui::GetCursorScreenPos();
  ImGui::PushID(component.source ? static_cast<const void*>(component.source)
                                 : static_cast<const void*>(&component));
  if (component.source) {
    ImGui::Selectable("##value", false, 0,
                      ImVec2{width, ImGui::GetTextLineHeight()});
    component.source->EmitDrag();
  } else {
    ImGui::Selectable("##value", false, ImGuiSelectableFlags_Disabled,
                      ImVec2{width, ImGui::GetTextLineHeight()});
  }
  ImGui::PopID();
  ImVec2 textMax{textPos.x + width, textPos.y + ImGui::GetTextLineHeight()};
  float signWidth = ImGui::CalcTextSize("+").x;
  ImVec2 signMax{textPos.x + signWidth, textMax.y};
  ImVec2 magnitudeMin{textPos.x + signWidth, textPos.y};
  ImVec2 magnitudeSize = ImGui::CalcTextSize(magnitude.c_str());

  if (!sign.empty()) {
    ImGui::RenderTextClipped(textPos, signMax, sign.c_str(), nullptr, nullptr,
                             ImVec2{0.0f, 0.0f});
  }
  ImGui::RenderTextClipped(magnitudeMin, textMax, magnitude.c_str(), nullptr,
                           &magnitudeSize, ImVec2{1.0f, 0.0f});
  ImGui::PopStyleColor();
}

static void DrawCompoundDataRow(
    std::string_view name, const std::array<DataRowComponent, 3>& components,
    std::string_view suffix = {}) {
  bool anyPresent = false;
  std::chrono::steady_clock::time_point latestUpdate;
  for (const auto& component : components) {
    if (component.present) {
      anyPresent = true;
      latestUpdate = std::max(latestUpdate, component.lastUpdate);
    }
  }

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::PushStyleColor(ImGuiCol_Text,
                        GetDataTextColor(anyPresent, latestUpdate));
  TextUnformatted(name);
  ImGui::PopStyleColor();
  ImGui::TableNextColumn();
  float componentWidth = GetCompoundComponentWidth();
  float componentSpacing = ImGui::GetStyle().ItemSpacing.x * 2.0f;
  float startX = ImGui::GetCursorPosX();
  for (size_t i = 0; i < components.size(); ++i) {
    if (i != 0) {
      ImGui::SameLine(0.0f, 0.0f);
      ImGui::SetCursorPosX(startX + i * (componentWidth + componentSpacing));
    }
    DrawDataRowComponent(components[i], componentWidth);
  }
  if (!suffix.empty()) {
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::SetCursorPosX(startX + components.size() * componentWidth +
                         (components.size() - 1) * componentSpacing +
                         ImGui::GetStyle().ItemSpacing.x);
    ImGui::PushStyleColor(ImGuiCol_Text,
                          GetDataTextColor(anyPresent, latestUpdate));
    TextUnformatted(suffix);
    ImGui::PopStyleColor();
  }
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

static void DrawDioLedRow(
    std::string_view name, const std::array<XRPDataField<bool>, 8>& values,
    const std::array<SourceSlot<wpi::glass::BooleanSource>, 8>& sourceSlots) {
  std::array<int, 8> ledValues;
  std::array<ImU32, 8> ledColors;
  std::array<wpi::glass::BooleanSource*, 8> sources;
  bool anyPresent = false;
  std::chrono::steady_clock::time_point latestUpdate;

  for (size_t i = 0; i < values.size(); ++i) {
    const auto& value = values[i];
    int colorIndex = static_cast<int>(i) + 1;
    ledValues[i] = value.value ? colorIndex : -colorIndex;
    ledColors[i] = GetDioLedColor(value.present, value.lastUpdate);
    sources[i] = sourceSlots[i].source.get();
    if (value.present) {
      anyPresent = true;
      latestUpdate = std::max(latestUpdate, value.lastUpdate);
    }
  }

  ImGui::TableNextRow();
  ImGui::PushStyleColor(ImGuiCol_Text,
                        GetDataTextColor(anyPresent, latestUpdate));
  ImGui::TableNextColumn();
  TextUnformatted(name);
  ImGui::TableNextColumn();
  wpi::glass::DrawLEDSources(
      ledValues.data(), sources.data(), static_cast<int>(ledValues.size()),
      static_cast<int>(ledValues.size()), ledColors.data());
  ImGui::PopStyleColor();
}

static void DrawControlDataTable(const XRPControlData& control) {
  if (!BeginXRPDataTable("XRPControlDataTable")) {
    return;
  }

  const auto& sources = gGui.dataSources;

  for (size_t i = 0; i < control.motors.size(); ++i) {
    const auto& motor = control.motors[i];
    DrawDataRow(MOTOR_LABELS[i], motor.present, motor.lastUpdate,
                sources.motors[i].source.get(),
                [&] { return FormatMotorOutput(motor.value); });
  }

  for (size_t i = 0; i < control.servos.size(); ++i) {
    const auto& servo = control.servos[i];
    DrawDataRow(SERVO_LABELS[i], servo.present, servo.lastUpdate,
                sources.servos[i].source.get(),
                [&] { return FormatServoOutput(servo.value); });
  }

  DrawDioLedRow("DIO outputs", control.digitalOutputs, sources.digitalOutputs);

  ImGui::EndTable();
}

static void DrawStatusDataTable(const XRPStatusData& status) {
  if (!BeginXRPDataTable("XRPStatusDataTable")) {
    return;
  }

  const auto& sources = gGui.dataSources;

  for (size_t i = 0; i < status.encoders.size(); ++i) {
    const auto& encoder = status.encoders[i];
    std::string countLabel = std::format("Encoder {} count", i);
    DrawDataRow(countLabel, encoder.present, encoder.lastUpdate,
                sources.encoderCounts[i].source.get(),
                [&] { return std::format("{}", encoder.value.count); });
    std::string periodLabel = std::format("Encoder {} period", i);
    DrawDataRow(
        periodLabel, encoder.present && encoder.value.periodValid,
        encoder.lastUpdate, sources.encoderPeriods[i].source.get(), [&] {
          return std::format("{} s", FormatSigned4(encoder.value.period));
        });
  }

  DrawDioLedRow("DIO inputs", status.digitalInputs, sources.digitalInputs);

  DrawCompoundDataRow("Gyro rate X/Y/Z",
                      {{{status.gyro.present, status.gyro.lastUpdate,
                         sources.gyroRates[0].source.get(),
                         FormatSigned3(status.gyro.value.rate.x)},
                        {status.gyro.present, status.gyro.lastUpdate,
                         sources.gyroRates[1].source.get(),
                         FormatSigned3(status.gyro.value.rate.y)},
                        {status.gyro.present, status.gyro.lastUpdate,
                         sources.gyroRates[2].source.get(),
                         FormatSigned3(status.gyro.value.rate.z)}}},
                      "deg/s");
  DrawCompoundDataRow("Gyro angle X/Y/Z",
                      {{{status.gyro.present, status.gyro.lastUpdate,
                         sources.gyroAngles[0].source.get(),
                         FormatSigned3(status.gyro.value.angle.x)},
                        {status.gyro.present, status.gyro.lastUpdate,
                         sources.gyroAngles[1].source.get(),
                         FormatSigned3(status.gyro.value.angle.y)},
                        {status.gyro.present, status.gyro.lastUpdate,
                         sources.gyroAngles[2].source.get(),
                         FormatSigned3(status.gyro.value.angle.z)}}},
                      "deg");
  DrawCompoundDataRow("Accel X/Y/Z",
                      {{{status.accel.present, status.accel.lastUpdate,
                         sources.accelerometer[0].source.get(),
                         FormatSigned3(status.accel.value.x)},
                        {status.accel.present, status.accel.lastUpdate,
                         sources.accelerometer[1].source.get(),
                         FormatSigned3(status.accel.value.y)},
                        {status.accel.present, status.accel.lastUpdate,
                         sources.accelerometer[2].source.get(),
                         FormatSigned3(status.accel.value.z)}}},
                      "G");
  DrawCompoundDataRow(
      "Analog 0/1/2",
      {{{status.analogInputs[0].present, status.analogInputs[0].lastUpdate,
         sources.analogInputs[0].source.get(),
         std::format("{:.3f}", status.analogInputs[0].value)},
        {status.analogInputs[1].present, status.analogInputs[1].lastUpdate,
         sources.analogInputs[1].source.get(),
         std::format("{:.3f}", status.analogInputs[1].value)},
        {status.analogInputs[2].present, status.analogInputs[2].lastUpdate,
         sources.analogInputs[2].source.get(),
         std::format("{:.3f}", status.analogInputs[2].value)}}},
      "V");

  ImGui::EndTable();
}

static void DrawXRPControlWindow() {
  DrawControlDataTable(gGui.data.control);
}

static void DrawXRPStatusWindow() {
  DrawStatusDataTable(gGui.data.status);
}

static void DrawXRPBluetoothWindow();

class XRPBluetoothView : public wpi::glass::View {
 public:
  void Display() override { DrawXRPBluetoothWindow(); }

  void Settings() override {
    ImGui::Checkbox("Show address", &gGui.showAddress);
  }

  bool HasSettings() override { return true; }
};

static void InitializeWindows() {
  if (gGui.windowManager || wpi::glass::GetCurrentContext() == nullptr) {
    return;
  }

  gGui.windowManager = std::make_unique<XRPWindowManager>(
      wpi::glass::GetStorageRoot().GetChild("XRP"));

  if (auto window = gGui.windowManager->AddWindow(
          XRP_BLUETOOTH_WINDOW_NAME, std::make_unique<XRPBluetoothView>())) {
    window->DisableRenamePopup();
    window->SetDefaultSize(430, 320);
  }
  if (auto window = gGui.windowManager->AddWindow(XRP_CONTROL_WINDOW_NAME,
                                                  DrawXRPControlWindow)) {
    window->DisableRenamePopup();
    window->SetDefaultSize(620, 260);
  }
  if (auto window = gGui.windowManager->AddWindow(XRP_STATUS_WINDOW_NAME,
                                                  DrawXRPStatusWindow)) {
    window->DisableRenamePopup();
    window->SetDefaultSize(620, 420);
  }
}

static void ShowXRPWindow(std::string_view name) {
  InitializeWindows();
  if (gGui.windowManager) {
    if (auto window = gGui.windowManager->GetWindow(name)) {
      window->SetVisible(true);
    }
  }
}

static void DrawXRPWindows() {
  InitializeWindows();
  if (gGui.windowManager) {
    gGui.windowManager->DisplayManagedWindows();
  }
}

static void DrawXRPMenuBarStatusControls();

static void DrawXRPMainMenu() {
  WithGuiContexts([] {
    InitializeWindows();
    if (ImGui::BeginMenu("XRP")) {
      if (gGui.windowManager) {
        gGui.windowManager->DisplayMenu();
      }
      ImGui::EndMenu();
    }
    DrawXRPMenuBarStatusControls();
  });
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

static bool HasBluetoothTarget() {
  return gGui.address[0] != '\0';
}

static bool IsConnectionActive(const XRPConnectionStatus& status) {
  return status.connected || status.connecting;
}

static bool CanConnectXRP(const XRPConnectionStatus& status,
                          bool commandRunning) {
  return status.supported && !commandRunning && HasBluetoothTarget() &&
         !IsConnectionActive(status);
}

static bool CanDisconnectXRP(const XRPConnectionStatus& status) {
  return status.connected || status.connecting;
}

static void ConnectXRP(HALSimXRP& simXRP) {
  std::string target = gGui.address;
  XRPBluetoothAddressType addressType = GetGuiAddressType();
  simXRP.ConnectBluetooth(target, addressType,
                          GetSelectedDeviceName(target, addressType));
}

static void DrawConnectionControls(HALSimXRP& simXRP,
                                   const XRPConnectionStatus& status,
                                   bool commandRunning) {
  bool pairingSupported =
      wpi::net::BluetoothLEPacketClient::IsPairingSupported();
  if (pairingSupported && gGui.showAddress) {
    ImGui::BeginDisabled(commandRunning || !HasBluetoothTarget() ||
                         IsConnectionActive(status));
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
  ImGui::BeginDisabled(!CanConnectXRP(status, commandRunning));
  if (ImGui::Button("Connect")) {
    ConnectXRP(simXRP);
  }
  ImGui::EndDisabled();

  ImGui::SameLine();
  ImGui::BeginDisabled(!CanDisconnectXRP(status));
  if (ImGui::Button("Disconnect")) {
    simXRP.DisconnectBluetooth();
  }
  ImGui::EndDisabled();
}

static void DrawXRPBluetoothWindow() {
  auto simXRP = gSimXRP.lock();
  if (!simXRP) {
    ImGui::TextUnformatted("XRP unavailable");
    return;
  }

  const auto& status = gGui.connectionStatus;
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
}

enum class XRPMenuIcon { CONNECT, DISCONNECT, BLUETOOTH };

static ImFont* GetXRPBluetoothFont() {
  ImFontAtlas* fonts = ImGui::GetIO().Fonts;
  if (gGui.bluetoothIconFontAtlas == fonts && gGui.bluetoothIconFont) {
    return gGui.bluetoothIconFont;
  }

  if (fonts->Locked || fonts->Fonts.empty()) {
    return nullptr;
  }

  static constexpr ImWchar glyphRanges[] = {
      FONT_AWESOME_BLUETOOTH_CODEPOINT, FONT_AWESOME_BLUETOOTH_CODEPOINT, 0};

  ImFontConfig fontConfig;
  std::snprintf(fontConfig.Name, sizeof(fontConfig.Name), "XRP Bluetooth Icon");
  gGui.bluetoothIconFont = ImGui::AddFontFontAwesomeBrands(
      ImGui::GetIO(), 0, &fontConfig, glyphRanges);
  gGui.bluetoothIconFontAtlas = fonts;
  return gGui.bluetoothIconFont;
}

static void DrawBluetoothFallbackIcon(ImDrawList* drawList, ImVec2 center,
                                      ImVec2 size, ImU32 color, float stroke) {
  ImVec2 top{center.x, center.y - size.y * 0.34f};
  ImVec2 bottom{center.x, center.y + size.y * 0.34f};
  ImVec2 rightTop{center.x + size.x * 0.22f, center.y - size.y * 0.18f};
  ImVec2 rightBottom{center.x + size.x * 0.22f, center.y + size.y * 0.18f};
  ImVec2 leftTop{center.x - size.x * 0.20f, center.y - size.y * 0.19f};
  ImVec2 leftBottom{center.x - size.x * 0.20f, center.y + size.y * 0.19f};

  drawList->AddLine(top, bottom, color, stroke);
  drawList->AddLine(top, rightTop, color, stroke);
  drawList->AddLine(rightTop, center, color, stroke);
  drawList->AddLine(center, rightBottom, color, stroke);
  drawList->AddLine(rightBottom, bottom, color, stroke);
  drawList->AddLine(leftTop, center, color, stroke);
  drawList->AddLine(center, leftBottom, color, stroke);
}

static void DrawXRPMenuIcon(ImDrawList* drawList, XRPMenuIcon icon, ImVec2 min,
                            ImVec2 max, ImU32 color) {
  ImVec2 size{max.x - min.x, max.y - min.y};
  ImVec2 center{(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f};
  float stroke = std::max(1.0f, size.y * 0.08f);

  if (icon == XRPMenuIcon::CONNECT) {
    ImVec2 p1{center.x - size.x * 0.18f, center.y - size.y * 0.24f};
    ImVec2 p2{center.x - size.x * 0.18f, center.y + size.y * 0.24f};
    ImVec2 p3{center.x + size.x * 0.24f, center.y};
    drawList->AddTriangleFilled(p1, p2, p3, color);
  } else if (icon == XRPMenuIcon::DISCONNECT) {
    ImVec2 rectMin{center.x - size.x * 0.19f, center.y - size.y * 0.19f};
    ImVec2 rectMax{center.x + size.x * 0.19f, center.y + size.y * 0.19f};
    drawList->AddRectFilled(rectMin, rectMax, color, stroke);
  } else {
    const char* iconText = ICON_FA_BLUETOOTH_B;
    float fontSize = ImGui::GetFontSize();
    if (ImFont* font = GetXRPBluetoothFont();
        font && font->GetFontBaked(fontSize)->FindGlyphNoFallback(
                    FONT_AWESOME_BLUETOOTH_CODEPOINT)) {
      ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, iconText);
      drawList->AddText(
          font, fontSize,
          {center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f}, color,
          iconText);
    } else {
      DrawBluetoothFallbackIcon(drawList, center, size, color, stroke);
    }
  }
}

static bool XRPMenuIconButton(const char* id, const char* tooltip, bool enabled,
                              XRPMenuIcon icon) {
  ImVec2 buttonSize{ImGui::GetFrameHeight(), ImGui::GetFrameHeight()};
  bool pressed = ImGui::InvisibleButton(id, buttonSize) && enabled;
  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  ImVec2 min = ImGui::GetItemRectMin();
  ImVec2 max = ImGui::GetItemRectMax();
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  if (enabled && (hovered || active)) {
    drawList->AddRectFilled(min, max,
                            ImGui::GetColorU32(active ? ImGuiCol_ButtonActive
                                                      : ImGuiCol_ButtonHovered),
                            ImGui::GetStyle().FrameRounding);
  }

  DrawXRPMenuIcon(
      drawList, icon, min, max,
      ImGui::GetColorU32(enabled ? ImGuiCol_Text : ImGuiCol_TextDisabled));

  if (hovered) {
    ImGui::SetTooltip("%s", tooltip);
  }

  return pressed;
}

static void DrawXRPConnectionBadge(const XRPConnectionStatus& status) {
  ImVec2 badgeSize{ImGui::GetFrameHeight(), ImGui::GetFrameHeight()};
  ImGui::InvisibleButton("##XRPConnectionBadge", badgeSize);
  bool hovered = ImGui::IsItemHovered();
  ImVec2 min = ImGui::GetItemRectMin();
  ImVec2 max = ImGui::GetItemRectMax();
  ImVec2 center{(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f};
  ImVec4 color = status.connected ? ImVec4{0.25f, 0.75f, 0.35f, 1.0f}
                                  : ImVec4{0.85f, 0.25f, 0.25f, 1.0f};
  ImGui::GetWindowDrawList()->AddCircleFilled(
      center, ImGui::GetFontSize() * 0.32f,
      ImGui::ColorConvertFloat4ToU32(color), 18);

  if (hovered) {
    const char* fallback = status.connected ? "connected" : "disconnected";
    ImGui::SetTooltip("XRP %s",
                      status.status.empty() ? fallback : status.status.c_str());
  }
}

static void DrawXRPMenuBarStatusControls() {
  const ImGuiStyle& style = ImGui::GetStyle();
  float buttonSize = ImGui::GetFrameHeight();
  float controlsWidth = buttonSize * 4.0f + style.ItemSpacing.x * 3.0f;
  float currentX = ImGui::GetCursorPosX();
  float targetX =
      ImGui::GetWindowWidth() - controlsWidth - style.WindowPadding.x;
  ImGui::SameLine(std::max(currentX + style.ItemSpacing.x, targetX));

  const auto& status = gGui.connectionStatus;
  bool commandRunning = gGui.pendingCommand.valid();
  auto simXRP = gSimXRP.lock();

  DrawXRPConnectionBadge(status);
  ImGui::SameLine();
  if (XRPMenuIconButton("##XRPConnect", "Connect XRP",
                        simXRP && CanConnectXRP(status, commandRunning),
                        XRPMenuIcon::CONNECT)) {
    ConnectXRP(*simXRP);
  }
  ImGui::SameLine();
  if (XRPMenuIconButton("##XRPDisconnect", "Disconnect XRP",
                        simXRP && CanDisconnectXRP(status),
                        XRPMenuIcon::DISCONNECT)) {
    simXRP->DisconnectBluetooth();
  }
  ImGui::SameLine();
  if (XRPMenuIconButton("##XRPBluetoothSettings", "XRP Bluetooth Settings",
                        true, XRPMenuIcon::BLUETOOTH)) {
    ShowXRPWindow(XRP_BLUETOOTH_WINDOW_NAME);
  }
}

static void DrawGuiImpl() {
  auto simXRP = gSimXRP.lock();
  if (!simXRP) {
    DrawXRPWindows();
    return;
  }

  UpdatePendingCommand(*simXRP);

  gGui.connectionStatus = simXRP->GetConnectionStatus();
  InitializeFromConnection(gGui.connectionStatus);
  gGui.data = simXRP->GetDataSnapshot();
  UpdateDataSources(gGui.data);
  UpdateLatencyHistory(gGui.connectionStatus);
  DrawXRPWindows();
}

static void DrawGui() {
  WithGuiContexts(DrawGuiImpl);
}

static void ExtensionListener(void*, const char* name, void* data) {
  std::string_view nameView{name};
  if (nameView == ADD_GUI_LATE_EXECUTE_NAME && !gLateExecuteRegistered) {
    auto addGuiLateExecute = reinterpret_cast<AddGuiLateExecuteFn>(data);
    addGuiLateExecute(DrawGui);
    gLateExecuteRegistered = true;
  } else if (nameView == ADD_MAIN_MENU_NAME && !gMainMenuRegistered) {
    auto addMainMenu = reinterpret_cast<AddMainMenuFn>(data);
    addMainMenu(DrawXRPMainMenu);
    gMainMenuRegistered = true;
  } else if (nameView == GET_IMGUI_CONTEXT_NAME) {
    gGetImguiContext = reinterpret_cast<GetImguiContextFn>(data);
  } else if (nameView == GET_IMPLOT_CONTEXT_NAME) {
    gGetImPlotContext = reinterpret_cast<GetImPlotContextFn>(data);
  } else if (nameView == GET_GLASS_CONTEXT_NAME) {
    gGetGlassContext = reinterpret_cast<GetGlassContextFn>(data);
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
