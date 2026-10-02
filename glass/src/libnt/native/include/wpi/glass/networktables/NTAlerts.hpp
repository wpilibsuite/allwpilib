// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "wpi/glass/other/Alerts.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringArrayTopic.hpp"

namespace wpi::glass {

class NTAlertsModel : public AlertsModel {
 public:
  static constexpr const char* TYPE = "Alerts";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTAlertsModel(std::string_view path);
  NTAlertsModel(wpi::nt::NetworkTableInstance inst, std::string_view path);

  const std::vector<AlertData>& GetAlerts() override { return m_alerts; }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::StringArraySubscriber m_infos;
  wpi::nt::StringArraySubscriber m_warnings;
  wpi::nt::StringArraySubscriber m_errors;

  std::vector<std::string> m_infosValue;
  std::vector<std::string> m_warningsValue;
  std::vector<std::string> m_errorsValue;
  std::vector<AlertData> m_alerts;
};

}  // namespace wpi::glass
