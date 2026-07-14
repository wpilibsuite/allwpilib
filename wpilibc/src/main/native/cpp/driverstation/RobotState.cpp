// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/RobotState.hpp"

#include <cstdint>
#include <format>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/hal/DriverStation.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/util/Color.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"

namespace {
struct Instance {
  // Op mode lookup
  Instance();
  wpi::util::mutex opModeMutex;
  wpi::util::DenseMap<int64_t, HAL_OpModeOption> opModes;
  bool userProgramStarted = false;

  std::string OpModeToString(int64_t id) {
    std::scoped_lock lock{opModeMutex};
    if (id == 0) {
      return "";
    }
    auto it = opModes.find(id);
    if (it != opModes.end()) {
      return std::string{wpi::util::to_string_view(&it->second.name)};
    }
    return std::format("<{}>", id);
  }
};
}  // namespace

Instance::Instance() {
  HAL_Initialize(500, 0);
}

static Instance& GetInstance() {
  static Instance instance;
  return instance;
}

static int64_t DoAddOpMode(wpi::RobotMode mode, std::string_view name,
                           std::string_view group, std::string_view description,
                           int32_t textColor, int32_t backgroundColor) {
  if (wpi::util::trim(name).empty()) {
    return 0;
  }

  WPI_String nameWpi = wpi::util::make_string(name);
  WPI_String groupWpi = wpi::util::make_string(group);
  WPI_String descriptionWpi = wpi::util::make_string(description);

  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.opModeMutex};
  std::string nameCopy{name};
  for (;;) {
    int64_t id = HAL_MakeOpModeId(static_cast<HAL_RobotMode>(mode),
                                  std::hash<std::string_view>{}(nameCopy));
    auto [it, isNew] = inst.opModes.try_emplace(
        id, HAL_OpModeOption{id, nameWpi, groupWpi, descriptionWpi, textColor,
                             backgroundColor});
    if (isNew) {
      return id;
    }
    if (HAL_OpMode_GetRobotMode(it->second.id) ==
            static_cast<HAL_RobotMode>(mode) &&
        wpi::util::to_string_view(&it->second.name) == name) {
      return 0;  // can't insert duplicate name
    }
    // collision, try again with space appended
    nameCopy += ' ';
  }
}

static int32_t ConvertColorToInt(const wpi::util::Color& color) {
  return ((static_cast<int32_t>(color.red * 255) & 0xff) << 16) |
         ((static_cast<int32_t>(color.green * 255) & 0xff) << 8) |
         (static_cast<int32_t>(color.blue * 255) & 0xff);
}

namespace wpi {
std::string RobotState::OpModeToString(int64_t id) {
  return GetInstance().OpModeToString(id);
}

int64_t RobotState::AddOpMode(RobotMode mode, std::string_view name,
                              std::string_view group,
                              std::string_view description,
                              const wpi::util::Color& textColor,
                              const wpi::util::Color& backgroundColor) {
  return DoAddOpMode(mode, name, group, description,
                     ConvertColorToInt(textColor),
                     ConvertColorToInt(backgroundColor));
}

int64_t RobotState::AddOpMode(RobotMode mode, std::string_view name,
                              std::string_view group,
                              std::string_view description) {
  return DoAddOpMode(mode, name, group, description, -1, -1);
}

int64_t RobotState::RemoveOpMode(RobotMode mode, std::string_view name) {
  if (wpi::util::trim(name).empty()) {
    return 0;
  }

  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.opModeMutex};
  // we have to loop over all entries to find the one with the correct name
  // because the of the unique ID generation scheme
  for (auto it = inst.opModes.begin(), end = inst.opModes.end(); it != end;
       ++it) {
    if (HAL_OpMode_GetRobotMode(it->second.id) ==
            static_cast<HAL_RobotMode>(mode) &&
        wpi::util::to_string_view(&it->second.name) == name) {
      int64_t id = it->second.id;
      inst.opModes.erase(it);
      return id;
    }
  }
  return 0;
}

void RobotState::PublishOpModes() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.opModeMutex};
  std::vector<HAL_OpModeOption> options;
  options.reserve(inst.opModes.size());
  for (auto&& [id, option] : inst.opModes) {
    options.emplace_back(option);
  }
  HAL_SetOpModeOptions(options.data(), options.size());

  int modeCounts[HAL_ROBOT_MODE_UTILITY + 1] = {0, 0, 0, 0};
  for (const auto& opMode : options) {
    ++modeCounts[HAL_OpMode_GetRobotMode(opMode.id)];
  }

  HAL_ReportUsage("OpMode/AUTONOMOUS",
                  std::to_string(modeCounts[HAL_ROBOT_MODE_AUTONOMOUS]));
  HAL_ReportUsage("OpMode/TELEOPERATED",
                  std::to_string(modeCounts[HAL_ROBOT_MODE_TELEOPERATED]));
  HAL_ReportUsage("OpMode/UTILITY",
                  std::to_string(modeCounts[HAL_ROBOT_MODE_UTILITY]));
  HAL_ReportUsage("OpMode/UNKNOWN",
                  std::to_string(modeCounts[HAL_ROBOT_MODE_UNKNOWN]));
}

void RobotState::ClearOpModes() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.opModeMutex};
  inst.opModes.clear();
  HAL_SetOpModeOptions(nullptr, 0);
}

void RobotState::ObserveUserProgramStarting() {
  ::GetInstance().userProgramStarted = true;
  HAL_ObserveUserProgramStarting();
}

int64_t RobotState::GetOpModeId() {
  if (!::GetInstance().userProgramStarted) {
    return 0;
  }

  return hal::GetControlWord().GetOpModeId();
}

std::string RobotState::GetOpMode() {
  if (!::GetInstance().userProgramStarted) {
    return "";
  }

  return GetInstance().OpModeToString(GetOpModeId());
}
}  // namespace wpi
