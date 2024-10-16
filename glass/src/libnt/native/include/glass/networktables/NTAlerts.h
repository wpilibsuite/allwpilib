// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <networktables/NetworkTableInstance.h>
#include <networktables/StringArrayTopic.h>

#include "glass/other/Alerts.h"

namespace glass {

class NTAlertsModel : public AlertsModel {
 public:
  static constexpr const char* kType = "Alerts";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTAlertsModel(std::string_view path);
  NTAlertsModel(nt::NetworkTableInstance inst, std::string_view path);

  const std::vector<std::string>& GetInfos() override { return m_infosValue; }

  const std::vector<std::string>& GetWarnings() override {
    return m_warningsValue;
  }

  const std::vector<std::string>& GetErrors() override { return m_errorsValue; }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  nt::NetworkTableInstance m_inst;
  nt::StringArraySubscriber m_infos;
  nt::StringArraySubscriber m_warnings;
  nt::StringArraySubscriber m_errors;

  std::vector<std::string> m_infosValue;
  std::vector<std::string> m_warningsValue;
  std::vector<std::string> m_errorsValue;
};

}  // namespace glass
