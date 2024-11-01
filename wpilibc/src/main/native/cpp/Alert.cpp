// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Alert.h"

#include <frc/RobotController.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <networktables/NTSendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;

// TODO: throw if matching (group, text, type) already constructed
Alert::Alert(std::string_view text, AlertType type)
    : Alert("Alerts", text, type) {}

Alert::Alert(std::string_view group, std::string_view text, AlertType type)
    : m_type(type), m_text(text), m_group{&GetGroupSendable(group)} {}

Alert::Alert(Alert&& other)
    : m_type{other.m_type},
      m_text{std::move(other.m_text)},
      m_group{std::exchange(other.m_group, nullptr)},
      m_active{std::exchange(other.m_active, false)},
      m_activeStartTime{other.m_activeStartTime} {}

Alert& Alert::operator=(Alert&& other) {
  if (&other != this) {
    // We want to destroy current state after the move is done
    Alert tmp{std::move(*this)};
    // Now, swap moved-from state with other state
    std::swap(m_type, other.m_type);
    std::swap(m_text, other.m_text);
    std::swap(m_group, other.m_group);
    std::swap(m_active, other.m_active);
    std::swap(m_activeStartTime, other.m_activeStartTime);
  }
  return *this;
}

Alert::~Alert() {
  Set(false);
}

void Alert::Set(bool active) {
  if (active == m_active) {
    return;
  }

  if (active) {
    m_activeStartTime = frc::RobotController::GetFPGATime();
    m_group->GetSetForType(m_type).emplace(m_activeStartTime, m_text);
  } else {
    m_group->GetSetForType(m_type).erase({m_activeStartTime, m_text});
  }
  m_active = active;
}

bool Alert::Get() const {
  return m_active;
}

void Alert::SetText(std::string_view text) {
  if (text == m_text) {
    return;
  }

  std::string oldText = std::move(m_text);
  m_text = text;

  if (m_active) {
    auto& set = m_group->GetSetForType(m_type);
    auto iter = set.find({m_activeStartTime, oldText});
    auto hint = set.erase(iter);
    set.emplace_hint(hint, m_activeStartTime, m_text);
  }
}

std::string Alert::GetText() const {
  return m_text;
}

Alert::SendableAlerts::SendableAlerts() {
  m_alerts.fill({});
}

void Alert::SendableAlerts::InitSendable(nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("Alerts");
  builder.AddStringArrayProperty(
      "errors", [this]() { return GetStrings(AlertType::kError); }, nullptr);
  builder.AddStringArrayProperty(
      "warnings", [this]() { return GetStrings(AlertType::kWarning); },
      nullptr);
  builder.AddStringArrayProperty(
      "infos", [this]() { return GetStrings(AlertType::kInfo); }, nullptr);
}

std::set<Alert::PublishedAlert>& Alert::SendableAlerts::GetSetForType(
    AlertType type) {
  return const_cast<std::set<Alert::PublishedAlert>&>(
      std::as_const(*this).GetSetForType(type));
}

const std::set<Alert::PublishedAlert>& Alert::SendableAlerts::GetSetForType(
    AlertType type) const {
  switch (type) {
    case AlertType::kInfo:
    case AlertType::kWarning:
    case AlertType::kError:
      return m_alerts[static_cast<int32_t>(type)];
    default:
      throw FRC_MakeError(frc::err::InvalidParameter, "Invalid Alert Type: {}",
                          type);
  }
}

std::vector<std::string> Alert::SendableAlerts::GetStrings(
    AlertType type) const {
  auto set = GetSetForType(type);
  std::vector<std::string> output;
  output.reserve(set.size());
  for (auto& alert : set) {
    output.emplace_back(alert.text);
  }
  return output;
}

Alert::SendableAlerts& Alert::GetGroupSendable(std::string_view group) {
  // Force initialization of SendableRegistry before our magic static to prevent
  // incorrect destruction order.
  wpi::SendableRegistry::EnsureInitialized();
  static wpi::StringMap<Alert::SendableAlerts> groups;

  auto [iter, exists] = groups.try_emplace(group);
  SendableAlerts& sendable = iter->second;
  if (!exists) {
    frc::SmartDashboard::PutData(group, &iter->second);
  }
  return sendable;
}

std::string frc::format_as(Alert::AlertType type) {
  switch (type) {
    case Alert::AlertType::kInfo:
      return "kInfo";
    case Alert::AlertType::kWarning:
      return "kWarning";
    case Alert::AlertType::kError:
      return "kError";
    default:
      return std::to_string(static_cast<int>(type));
  }
}
