// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/hal/Alert.h"
#include "wpi/hal/Types.hpp"

namespace wpi {

/**
 * Persistent alert to be sent to the driver station. Alerts are tagged with a
 * type of HIGH, MEDIUM, or LOW to denote urgency. See
 * Alert::Level for suggested usage of each type. Alerts can be displayed on
 * supported dashboards, and are shown in a priority order based on type and
 * recency of activation, with newly activated alerts first.
 *
 * Alerts should be created once and stored persistently, then updated to
 * "active" or "inactive" as necessary. Set(bool) can be safely called
 * periodically.
 *
 * <pre>
 * class Robot {
 *   wpi::Alert alert{"Something went wrong", wpi::Alert::Level::MEDIUM};
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
  enum class Level {
    /**
     * High priority alert - displayed first with a red "X"
     * symbol. Use this type for problems which will seriously affect the
     * robot's functionality and thus require immediate attention.
     */
    HIGH = HAL_ALERT_HIGH,

    /**
     * Medium priority alert - displayed second with a yellow "!" symbol.
     * Use this type for problems which could affect the robot's functionality
     * but do not necessarily require immediate attention.
     */
    MEDIUM = HAL_ALERT_MEDIUM,

    /**
     * Low priority alert - displayed last with a green "i" symbol. Use this
     * type for problems which are unlikely to affect the robot's functionality,
     * or any other alerts which do not fall under the other categories.
     */
    LOW = HAL_ALERT_LOW,
  };

  /**
   * Creates a new alert in the default group - "Alerts". If this is the first
   * to be instantiated, the appropriate entries will be added to NetworkTables.
   *
   * @param text Text to be displayed when the alert is active.
   * @param level Alert urgency level.
   */
  Alert(std::string_view text, Level level);

  /**
   * Creates a new alert. If this is the first to be instantiated in its group,
   * the appropriate entries will be added to NetworkTables.
   *
   * @param group Group identifier, used as the entry name in NetworkTables.
   * @param text Text to be displayed when the alert is active.
   * @param level Alert urgency level.
   */
  Alert(std::string_view group, std::string_view text, Level level);

  /**
   * Sets whether the alert should currently be displayed. This method can be
   * safely called periodically.
   *
   * @param active Whether to display the alert.
   */
  void Set(bool active);

  /**
   * Gets whether the alert is active.
   * @return whether the alert is active.
   */
  bool Get() const;

  /**
   * Updates current alert text. Use this method to dynamically change the
   * displayed alert, such as including more details about the detected problem.
   *
   * @param text Text to be displayed when the alert is active.
   */
  void SetText(std::string_view text);

  /**
   * Gets the current alert text.
   * @return the current text.
   */
  std::string GetText() const;

  /**
   * Get the type of this alert.
   * @return the type
   */
  Level GetType() const { return m_type; }

 private:
  Level m_type;
  wpi::hal::Handle<HAL_AlertHandle, HAL_DestroyAlert> m_handle;
};

}  // namespace wpi
