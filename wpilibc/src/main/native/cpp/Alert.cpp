// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Alert.h"

#include <set>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <networktables/NTSendable.h>
#include <networktables/NTSendableBuilder.h>
#include <wpi/StringMap.h>
#include <wpi/sendable/SendableHelper.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/RobotController.h"
#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;

class Alert::PublishedAlert {
 public:
  PublishedAlert(uint64_t timestamp, std::string_view text)
      : timestamp{timestamp}, text{text} {}
  uint64_t timestamp;
  std::string text;
  auto operator<=>(const PublishedAlert& other) const {
    if (timestamp != other.timestamp) {
      return other.timestamp <=> timestamp;
    } else {
      return text <=> other.text;
    }
  }
};

class Alert::SendableAlerts : public nt::NTSendable,
                              public wpi::SendableHelper<SendableAlerts> {
 public:
  SendableAlerts() { m_alerts.fill({}); }

  void InitSendable(nt::NTSendableBuilder& builder) override {
    builder.SetSmartDashboardType("Alerts");
    builder.AddStringArrayProperty(
        "errors", [this]() { return GetStrings(AlertType::kError); }, nullptr);
    builder.AddStringArrayProperty(
        "warnings", [this]() { return GetStrings(AlertType::kWarning); },
        nullptr);
    builder.AddStringArrayProperty(
        "infos", [this]() { return GetStrings(AlertType::kInfo); }, nullptr);
  }

  /**
   * Returns a reference to the set of active alerts for the given type.
   * @param type the type
   * @return reference to the set of active alerts for the type
   */
  std::set<PublishedAlert>& GetActiveAlertsStorage(AlertType type) {
    return const_cast<std::set<Alert::PublishedAlert>&>(
        std::as_const(*this).GetActiveAlertsStorage(type));
  }

  const std::set<PublishedAlert>& GetActiveAlertsStorage(AlertType type) const {
    switch (type) {
      case AlertType::kInfo:
      case AlertType::kWarning:
      case AlertType::kError:
        return m_alerts[static_cast<int32_t>(type)];
      default:
        throw FRC_MakeError(frc::err::InvalidParameter,
                            "Invalid Alert Type: {}", type);
    }
  }

  /**
   * Returns the SendableAlerts for a given group, initializing and publishing
   * if it does not already exist.
   * @param group the group name
   * @return the SendableAlerts for the group
   */
  static SendableAlerts& ForGroup(std::string_view group) {
    // Force initialization of SendableRegistry before our magic static to
    // prevent incorrect destruction order.
    wpi::SendableRegistry::EnsureInitialized();
    static wpi::StringMap<Alert::SendableAlerts> groups;

    auto [iter, inserted] = groups.try_emplace(group);
    SendableAlerts& sendable = iter->second;
    if (inserted) {
      frc::SmartDashboard::PutData(group, &iter->second);
    }
    return sendable;
  }

 private:
  std::vector<std::string> GetStrings(AlertType type) const {
    auto& set = GetActiveAlertsStorage(type);
    std::vector<std::string> output;
    output.reserve(set.size());
    for (auto& alert : set) {
      output.emplace_back(alert.text);
    }
    return output;
  }

  std::array<std::set<PublishedAlert>, 3> m_alerts;
};

Alert::Alert(std::string_view text, AlertType type)
    : Alert("Alerts", text, type) {}

Alert::Alert(std::string_view group, std::string_view text, AlertType type)
    : m_type(type),
      m_text(text),
      m_activeAlerts{
          &SendableAlerts::ForGroup(group).GetActiveAlertsStorage(m_type)} {}

Alert::Alert(Alert&& other)
    : m_type{other.m_type},
      m_text{std::move(other.m_text)},
      m_activeAlerts{std::exchange(other.m_activeAlerts, nullptr)},
      m_active{std::exchange(other.m_active, false)},
      m_activeStartTime{other.m_activeStartTime} {}

Alert& Alert::operator=(Alert&& other) {
  if (&other != this) {
    // We want to destroy current state after the move is done
    Alert tmp{std::move(*this)};
    // Now, swap moved-from state with other state
    std::swap(m_type, other.m_type);
    std::swap(m_text, other.m_text);
    std::swap(m_activeAlerts, other.m_activeAlerts);
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
    m_activeStartTime = frc::RobotController::GetTime();
    m_activeAlerts->emplace(m_activeStartTime, m_text);
  } else {
    m_activeAlerts->erase({m_activeStartTime, m_text});
  }
  m_active = active;
}

void Alert::SetText(std::string_view text) {
  if (text == m_text) {
    return;
  }

  std::string oldText = std::move(m_text);
  m_text = text;

  if (m_active) {
    auto iter = m_activeAlerts->find({m_activeStartTime, oldText});
    auto hint = m_activeAlerts->erase(iter);
    m_activeAlerts->emplace_hint(hint, m_activeStartTime, m_text);
  }
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
