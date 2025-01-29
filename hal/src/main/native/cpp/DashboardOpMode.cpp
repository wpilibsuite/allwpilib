// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/DashboardOpMode.hpp"

#include <atomic>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "wpi/hal/DriverStationTypes.h"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"
#include "wpi/nt/StringArrayTopic.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.h"

using namespace wpi;

namespace {
class DashboardOpModeSender {
 public:
  void Start(nt::NetworkTableInstance inst, std::string_view tableName) {
    m_typeTopic = inst.GetStringTopic(fmt::format("{}/.type", tableName));
    m_optionsTopic =
        inst.GetStringArrayTopic(fmt::format("{}/options", tableName));
    m_activeTopic = inst.GetStringTopic(fmt::format("{}/active", tableName));
    m_selectedSub = inst.GetStringTopic(fmt::format("{}/selected", tableName))
                        .Subscribe("");
    m_selectedListener = nt::NetworkTableListener::CreateListener(
        m_selectedSub, NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE,
        [this](const nt::Event& event) {
          if (auto data = event.GetValueEventData()) {
            if (data->value.IsString()) {
              m_activePub.Set(data->value.GetString());
            }
          }
        });
  }

  void Enable() {
    m_typePub = m_typeTopic.Publish();
    m_typePub.Set("String Chooser");

    m_optionsPub = m_optionsTopic.Publish();
    m_optionsPub.Set(m_options);

    m_activePub = m_activeTopic.Publish();
    m_activePub.Set("");
  }

  void SetOptions(std::span<const HAL_OpModeOption> options,
                  HAL_RobotMode mode) {
    m_optionMap.clear();
    m_options.clear();
    for (auto&& option : options) {
      if (HAL_OpMode_GetRobotMode(option.id) == mode) {
        auto name = util::to_string_view(&option.name);
        m_optionMap[name] = option.id;
        m_options.emplace_back(name);
      }
    }
    if (m_optionsPub) {
      m_optionsPub.Set(m_options);
    }
  }

  int64_t GetSelected() const {
    auto it = m_optionMap.find(m_selectedSub.Get());
    if (it == m_optionMap.end()) {
      return 0;
    }
    return it->second;
  }

 private:
  nt::StringTopic m_typeTopic;
  nt::StringPublisher m_typePub;

  nt::StringArrayTopic m_optionsTopic;
  nt::StringArrayPublisher m_optionsPub;

  nt::StringTopic m_activeTopic;
  nt::StringPublisher m_activePub;

  nt::StringSubscriber m_selectedSub;
  nt::NetworkTableListener m_selectedListener;

  util::StringMap<int64_t> m_optionMap;
  std::vector<std::string> m_options;
};

struct DashboardOpModeInstance {
  void Start(nt::NetworkTableInstance inst) {
    autoOpModes.Start(inst, "/SmartDashboard/Auto OpMode");
    teleopOpModes.Start(inst, "/SmartDashboard/Teleop OpMode");
    testOpModes.Start(inst, "/SmartDashboard/Test OpMode");
  }

  util::mutex mutex;
  DashboardOpModeSender autoOpModes;
  DashboardOpModeSender teleopOpModes;
  DashboardOpModeSender testOpModes;
};
}  // namespace

static DashboardOpModeInstance* gInstance;
static std::atomic_flag gStarted{};
static std::atomic_flag gEnabled{};

void hal::InitializeDashboardOpMode() {
  static DashboardOpModeInstance inst;
  gInstance = &inst;
}

void hal::SetDashboardOpModeOptions(std::span<const HAL_OpModeOption> options) {
  std::scoped_lock lock{gInstance->mutex};
  gInstance->autoOpModes.SetOptions(options, HAL_ROBOTMODE_AUTONOMOUS);
  gInstance->teleopOpModes.SetOptions(options, HAL_ROBOTMODE_TELEOPERATED);
  gInstance->testOpModes.SetOptions(options, HAL_ROBOTMODE_TEST);
}

void hal::StartDashboardOpMode() {
  if (gStarted.test_and_set()) {
    return;
  }
  std::scoped_lock lock{gInstance->mutex};
  gInstance->Start(nt::NetworkTableInstance::GetDefault());
}

void hal::EnableDashboardOpMode() {
  if (gEnabled.test_and_set()) {
    return;
  }
  StartDashboardOpMode();
  std::scoped_lock lock{gInstance->mutex};
  gInstance->autoOpModes.Enable();
  gInstance->teleopOpModes.Enable();
  gInstance->testOpModes.Enable();
}

int64_t hal::GetDashboardSelectedOpMode(HAL_RobotMode robotMode) {
  if (!gEnabled.test()) {
    return 0;
  }
  std::scoped_lock lock{gInstance->mutex};
  switch (robotMode) {
    case HAL_ROBOTMODE_AUTONOMOUS:
      return gInstance->autoOpModes.GetSelected();
    case HAL_ROBOTMODE_TELEOPERATED:
      return gInstance->teleopOpModes.GetSelected();
    case HAL_ROBOTMODE_TEST:
      return gInstance->testOpModes.GetSelected();
    default:
      return 0;
  }
}
