// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AlertSim.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "wpi/util/Alert.h"
#include "wpi/util/string.hpp"

using namespace wpi;
using namespace wpi::sim;

int32_t AlertSim::GetCount() {
  return std::max(WPI_GetNumAlerts(), 0);
}

std::vector<AlertSim::AlertInfo> AlertSim::GetAll() {
  int32_t allocLen = WPI_GetNumAlerts();
  if (allocLen <= 0) {
    return {};
  }
  WPI_AlertInfo* cInfos = new WPI_AlertInfo[allocLen];
  int32_t len = WPI_GetAlerts(cInfos, allocLen);
  if (len <= 0) {
    delete[] cInfos;
    return {};
  }
  std::vector<AlertInfo> infos;
  infos.reserve(len);
  for (int32_t i = 0; i < len; ++i) {
    const auto& cInfo = cInfos[i];
    infos.emplace_back(std::string{wpi::util::to_string_view(&cInfo.group)},
                       std::string{wpi::util::to_string_view(&cInfo.id)},
                       std::string{wpi::util::to_string_view(&cInfo.text)},
                       cInfo.activeStartTime,
                       static_cast<wpi::util::Alert::Level>(cInfo.level));
  }
  WPI_FreeAlerts(cInfos, len < allocLen ? len : allocLen);
  delete[] cInfos;
  return infos;
}

std::vector<AlertSim::AlertInfo> AlertSim::GetActive() {
  auto infos = GetAll();
  std::erase_if(infos, [](const AlertInfo& info) { return !info.isActive(); });
  return infos;
}

void AlertSim::ResetData() {
  WPI_ResetAlertData();
}
