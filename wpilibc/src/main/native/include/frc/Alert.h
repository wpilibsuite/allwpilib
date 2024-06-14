// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <networktables/NTSendable.h>
#include <units/time.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {

/**
 * Persistent alert to be sent via NetworkTables. Alerts are tagged with a type
 * of kError, kWarning, or kInfo to denote urgency. See Alert::AlertType for
 * suggested usage of each type. Alerts can be displayed on supported
 * dashboards, and are shown in a priority order based on type and recency of
 * activation.
 *
 * Alerts should be created once and stored persistently, then updated to
 * "active" or "inactive" as necessary. Set(bool) can be safely called
 * periodically.
 *
 * <pre>
 * class Robot {
 *   frc::Alert alert{"Something went wrong", frc::Alert::AlertType::kWarning};
 * }
 *
 * Robot::periodic() {
 *   alert.Set(...);
 * }
 * </pre>
 */
class Alert {
 public:
  /**
   * Represents an alert's level of urgency.
   */
  enum class AlertType {
    /**
     * High priority alert - displayed first on the dashboard with a red "X"
     * symbol. Use this type for problems which will seriously affect the
     * robot's functionality and thus require immediate attention.
     */
    kError,

    /**
     * Medium priority alert - displayed second on the dashboard with a yellow
     * "!" symbol. Use this type for problems which could affect the robot's
     * functionality but do not necessarily require immediate attention.
     */
    kWarning,

    /**
     * Low priority alert - displayed last on the dashboard with a green "i"
     * symbol. Use this type for problems which are unlikely to affect the
     * robot's functionality, or any other alerts which do not fall under the
     * other categories.
     */
    kInfo
  };

  /**
   * Creates a new alert in the default group - "Alerts". If this is the first
   * to be instantiated, the appropriate entries will be added to NetworkTables.
   *
   * @param text Text to be displayed when the alert is active.
   * @param type Alert urgency level.
   */
  Alert(std::string_view text, AlertType type);

  /**
   * Creates a new alert. If this is the first to be instantiated in its group,
   * the appropriate entries will be added to NetworkTables.
   *
   * @param group Group identifier, used as the entry name in NetworkTables.
   * @param text Text to be displayed when the alert is active.
   * @param type Alert urgency level.
   */
  Alert(std::string_view group, std::string_view text, AlertType type);

  /**
   * Sets whether the alert should currently be displayed. When activated, the
   * alert text will also be sent to the console. This method can be safely
   * called periodically.
   *
   * @param active Whether to display the alert.
   */
  void Set(bool active);

  /**
   * Updates current alert text. Use this method to dynamically change the
   * displayed alert, such as including more details about the detected problem.
   *
   * @param text Text to be displayed when the alert is active.
   */
  void SetText(std::string_view text);

 private:
  AlertType m_type;
  bool m_active = false;
  units::second_t m_activeStartTime;
  std::string_view m_text;

  class SendableAlerts : public nt::NTSendable,
                         public wpi::SendableHelper<SendableAlerts> {
   public:
    std::vector<std::shared_ptr<Alert>> m_alerts;
    void InitSendable(nt::NTSendableBuilder& builder) override;

   private:
    std::vector<std::string> GetStrings(AlertType type);
  };

  static std::map<std::string_view, SendableAlerts> groups;
};

}  // namespace frc
