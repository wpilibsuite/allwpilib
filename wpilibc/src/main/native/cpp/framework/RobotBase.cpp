// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/RobotBase.hpp"

#include <stdint.h>

#include <atomic>
#include <cstdio>
#include <format>
#include <memory>
#include <mutex>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "wpi/backend/NetworkTablesTelemetryBackend.hpp"
#include "wpi/backend/NetworkTablesTunableBackend.hpp"
#include "wpi/driverstation/RobotState.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/WPILibVersion.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/util/Alert.hpp"
#include "wpi/util/UsageReporting.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/timestamp.hpp"

static_assert(wpi::RuntimeType::SYSTEMCORE ==
              static_cast<wpi::RuntimeType>(HAL_RUNTIME_SYSTEMCORE));
static_assert(wpi::RuntimeType::SIMULATION ==
              static_cast<wpi::RuntimeType>(HAL_RUNTIME_SIMULATION));

using namespace wpi;

int wpi::RunHALInitialization() {
  if (!HAL_Initialize()) {
    std::puts("FATAL ERROR: HAL could not be initialized");
    return -1;
  }
  wpi::util::SetReportUsageImpl(
      static_cast<WPI_ReportUsageImpl>(HAL_ReportUsage));
  wpi::internal::DriverStationBackend::RefreshData();
  wpi::util::ReportUsage("Language", "C++");
  wpi::util::ReportUsage("WPILibVersion", GetWPILibVersion());
  HAL_PublishWpilibVersion(std::format("{} (C++)", GetWPILibVersion()));

  std::puts("\n********** Robot program starting **********");
  return 0;
}

std::thread::id RobotBase::m_threadId;

namespace {
std::atomic<uint32_t> s_nextWarningReporterId{0};

void PrintDefaultTelemetryWarning(std::string_view path, std::string_view msg) {
  std::print(stderr, "Telemetry '{}': warning: {}\n", path, msg);
}

void PrintDefaultTunableWarning(std::string_view msg) {
  std::print(stderr, "Tunable warning: {}\n", msg);
}

class WPILibMathShared : public wpi::math::MathShared {
 public:
  void ReportErrorV(std::string_view format, std::format_args args) override {
    wpi::ReportErrorV(err::Error, __FILE__, __LINE__, __FUNCTION__, format,
                      args);
  }

  void ReportWarningV(std::string_view format, std::format_args args) override {
    wpi::ReportErrorV(warn::Warning, __FILE__, __LINE__, __FUNCTION__, format,
                      args);
  }

  wpi::units::seconds<> GetTimestamp() override {
    return wpi::units::seconds<>{wpi::util::Now() * 1.0e-9};
  }
};
}  // namespace

struct wpi::RobotBase::WarningReporter {
  WarningReporter()
      : m_alertIdPrefix{std::format("warning{}_", s_nextWarningReporterId++)} {}

  void ReportTelemetry(std::string_view path, std::string_view msg) {
    std::string key{path};
    key.push_back('\n');
    key.append(msg.data(), msg.size());
    Report(m_telemetryAlerts, "Telemetry", key,
           std::format("Telemetry '{}': warning: {}", path, msg));
  }

  void ReportTunable(std::string_view msg) {
    Report(m_tunableAlerts, "Tunables", msg,
           std::format("Tunable warning: {}", msg));
  }

 private:
  void Report(std::unordered_map<std::string, wpi::util::Alert>& alerts,
              std::string_view group, std::string_view key,
              std::string_view text) {
    std::scoped_lock lock{m_mutex};
    auto [it, inserted] = alerts.try_emplace(std::string{key});
    if (inserted || !it->second) {
      it->second = wpi::util::Alert{
          group, std::format("{}{}", m_alertIdPrefix, m_nextAlertId++), text,
          wpi::util::Alert::Level::MEDIUM};
      if (!it->second) {
        alerts.erase(it);
        return;
      }
    }

    it->second.SetText(text);
    it->second.Set(true);
  }

  wpi::util::mutex m_mutex;
  std::string m_alertIdPrefix;
  uint32_t m_nextAlertId = 0;
  std::unordered_map<std::string, wpi::util::Alert> m_telemetryAlerts;
  std::unordered_map<std::string, wpi::util::Alert> m_tunableAlerts;
};

static void SetupMathShared() {
  wpi::math::MathSharedStore::SetMathShared(
      std::make_unique<WPILibMathShared>());
}

bool RobotBase::IsEnabled() {
  return RobotState::IsEnabled();
}

bool RobotBase::IsDisabled() {
  return RobotState::IsDisabled();
}

bool RobotBase::IsAutonomous() {
  return RobotState::IsAutonomous();
}

bool RobotBase::IsAutonomousEnabled() {
  return RobotState::IsAutonomousEnabled();
}

bool RobotBase::IsTeleop() {
  return RobotState::IsTeleop();
}

bool RobotBase::IsTeleopEnabled() {
  return RobotState::IsTeleopEnabled();
}

bool RobotBase::IsUtility() {
  return RobotState::IsUtility();
}

bool RobotBase::IsUtilityEnabled() {
  return RobotState::IsUtilityEnabled();
}

int64_t RobotBase::GetOpModeId() {
  return RobotState::GetOpModeId();
}

std::string RobotBase::GetOpMode() {
  return RobotState::GetOpMode();
}

std::thread::id RobotBase::GetThreadId() {
  return m_threadId;
}

RuntimeType RobotBase::GetRuntimeType() {
  return static_cast<RuntimeType>(HAL_GetRuntimeType());
}

RobotBase::RobotBase() {
  m_threadId = std::this_thread::get_id();

  SetupMathShared();
  m_warningReporter = std::make_shared<WarningReporter>();
  std::weak_ptr<WarningReporter> warningReporter{m_warningReporter};

  auto inst = wpi::nt::NetworkTableInstance::GetDefault();
  // subscribe to "" to force persistent values to propagate to local
  wpi::nt::SubscribeMultiple(inst.GetHandle(), {{std::string_view{}}},
                             {.disableSignal = true});
  if constexpr (!IsSimulation()) {
    inst.StartServer("/home/systemcore/networktables.json", "", "robot");
  } else {
    inst.StartServer("networktables.json", "", "robot");
  }

  wpi::telemetry::TelemetryRegistry::SetReportWarning(
      [warningReporter](std::string_view path, std::string_view msg) {
        if (auto reporter = warningReporter.lock()) {
          reporter->ReportTelemetry(path, msg);
        } else {
          PrintDefaultTelemetryWarning(path, msg);
        }
      });
  wpi::tunables::TunableRegistry::SetReportWarning(
      [warningReporter](std::string_view msg) {
        if (auto reporter = warningReporter.lock()) {
          reporter->ReportTunable(msg);
        } else {
          PrintDefaultTunableWarning(msg);
        }
      });

  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<backend::NetworkTablesTelemetryBackend>(
              inst, "/Telemetry"));
  wpi::tunables::TunableRegistry::RegisterBackend(
      "", std::make_shared<backend::NetworkTablesTunableBackend>(inst,
                                                                 "/Tunables"));

  // wait for the NT server to actually start
  int count = 0;
  while ((inst.GetNetworkMode() & NT_NET_MODE_STARTING) != 0) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10ms);
    ++count;
    if (count > 100) {
      wpi::util::print(stderr,
                       "timed out while waiting for NT server to start\n");
      break;
    }
  }

  m_programStartTimePublisher =
      inst.GetIntegerTopic("/Robot/ProgramStartTime").Publish();
  m_programStartTimePublisher.Set(
      static_cast<int64_t>(wpi::util::GetProgramStartTime()));

  connListenerHandle =
      inst.AddConnectionListener(false, [&](const wpi::nt::Event& event) {
        if (event.Is(wpi::nt::EventFlags::CONNECTED)) {
          auto connInfo = event.GetConnectionInfo();
          wpi::util::ReportUsage(std::format("NT/{}", connInfo->remote_id), "");
        }
      });

  // Call wpi::internal::DriverStationBackend::RefreshData() to kick things off
  wpi::internal::DriverStationBackend::RefreshData();
}

RobotBase::~RobotBase() {
  m_warningReporter.reset();
}
