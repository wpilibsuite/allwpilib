// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DashboardOpMode.hpp"

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
  void Enable(nt::NetworkTableInstance inst, std::string_view tableName) {
    m_typePub =
        inst.GetStringTopic(fmt::format("{}/.type", tableName)).Publish();
    m_typePub.Set("String Chooser");

    m_optionsPub =
        inst.GetStringArrayTopic(fmt::format("{}/options", tableName))
            .Publish();
    m_optionsPub.Set(m_options);

    m_activePub =
        inst.GetStringTopic(fmt::format("{}/active", tableName)).Publish();
    m_activePub.Set("");

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
  nt::StringPublisher m_typePub;
  nt::StringArrayPublisher m_optionsPub;
  nt::StringPublisher m_activePub;
  nt::StringSubscriber m_selectedSub;
  nt::NetworkTableListener m_selectedListener;
  util::StringMap<int64_t> m_optionMap;
  std::vector<std::string> m_options;
};

struct DashboardOpModeInstance {
  util::mutex mutex;
  DashboardOpModeSender autoOpModes;
  DashboardOpModeSender teleopOpModes;
  DashboardOpModeSender testOpModes;
};
}  // namespace

static DashboardOpModeInstance& GetInstance() {
  static DashboardOpModeInstance inst;
  return inst;
}

void hal::SetDashboardOpModeOptions(std::span<const HAL_OpModeOption> options) {
  auto& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  inst.autoOpModes.SetOptions(options, HAL_ROBOTMODE_AUTONOMOUS);
  inst.teleopOpModes.SetOptions(options, HAL_ROBOTMODE_TELEOPERATED);
  inst.testOpModes.SetOptions(options, HAL_ROBOTMODE_TEST);
}

void hal::EnableDashboardOpMode() {
  auto& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  auto ntInst = nt::NetworkTableInstance::GetDefault();
  inst.autoOpModes.Enable(ntInst, "/SmartDashboard/Auto Op Modes");
  inst.teleopOpModes.Enable(ntInst, "/SmartDashboard/Teleop Op Modes");
  inst.testOpModes.Enable(ntInst, "/SmartDashboard/Test Op Modes");
}

int64_t hal::GetDashboardSelectedOpMode(HAL_RobotMode robotMode) {
  auto& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  switch (robotMode) {
    case HAL_ROBOTMODE_AUTONOMOUS:
      return inst.autoOpModes.GetSelected();
    case HAL_ROBOTMODE_TELEOPERATED:
      return inst.teleopOpModes.GetSelected();
    case HAL_ROBOTMODE_TEST:
      return inst.testOpModes.GetSelected();
    default:
      return 0;
  }
}
