// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Alert.h"

#include <frc/Timer.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include <iostream>
#include <string>

#include <networktables/NTSendable.h>
#include <networktables/NTSendableBuilder.h>
#include <units/time.h>
#include <wpi/sendable/SendableHelper.h>

using namespace frc;

Alert::Alert(std::string_view text, AlertType type)
    : Alert("Alerts", text, type) {}

std::map<std::string_view, Alert::SendableAlerts> Alert::groups;

Alert::Alert(std::string_view group, std::string_view text, AlertType type)
    : m_type(type), m_text(text) {
  if (!groups.contains(group)) {
    groups[group] = SendableAlerts();
    frc::SmartDashboard::PutData(group, &groups[group]);
  }
  groups[group].m_alerts.push_back(std::shared_ptr<Alert>(this));
}

void Alert::Set(bool active) {
  if (active && !m_active) {
    m_activeStartTime = frc::Timer::GetFPGATimestamp();
  }
  m_active = active;
}

void Alert::SetText(std::string_view text) {
  m_text = text;
}

void Alert::SendableAlerts::InitSendable(nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("Alerts");
  builder.AddStringArrayProperty(
      "errors", [this]() mutable { return GetStrings(AlertType::kError); },
      nullptr);
  builder.AddStringArrayProperty(
      "warnings", [this]() mutable { return GetStrings(AlertType::kWarning); },
      nullptr);
  builder.AddStringArrayProperty(
      "infos", [this]() mutable { return GetStrings(AlertType::kInfo); },
      nullptr);
}

std::vector<std::string> Alert::SendableAlerts::GetStrings(AlertType type) {
  std::vector<std::shared_ptr<Alert>> alerts;
  for (auto alert : m_alerts) {
    if (alert->m_active && alert->m_type == type) {
      alerts.push_back(alert);
    }
  }
  std::sort(alerts.begin(), alerts.end(), [](const auto a, const auto b) {
    return a->m_activeStartTime > b->m_activeStartTime;
  });
  std::vector<std::string> output;
  for (auto alert : alerts) {
    std::string text{alert->m_text};
    output.push_back(text);
  }
  return output;
}
