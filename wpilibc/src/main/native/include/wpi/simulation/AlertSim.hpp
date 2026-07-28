// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "wpi/driverstation/Alert.hpp"
#include "wpi/hal/Types.h"

namespace wpi::sim {

/**
 * Class to get info on simulated alerts.
 */
class AlertSim final {
 public:
  AlertSim() = delete;

  /** Information about an alert. */
  struct AlertInfo {
    /** The handle of the alert. */
    HAL_AlertHandle handle;

    /** The group of the alert. */
    std::string group;

    /** The text of the alert. */
    std::string text;

    /** The time the alert became active. 0 if not active. */
    int64_t activeStartTime;

    /** The level of the alert (HIGH, MEDIUM, or LOW). */
    Alert::Level level;

    /**
     * Returns whether the alert is currently active.
     *
     * @return true if the alert is active, false otherwise
     */
    bool isActive() const { return activeStartTime != 0; }
  };

  /**
   * Gets the number of alerts. Note: this is not guaranteed to be consistent
   * with the number of alerts returned by GetAll.
   *
   * @return the number of alerts
   */
  static int32_t GetCount();

  /**
   * Gets detailed information about each alert (including inactive ones).
   *
   * @return Alerts
   */
  static std::vector<AlertInfo> GetAll();

  /**
   * Gets detailed information about all active alerts.
   *
   * @return Alerts
   */
  static std::vector<AlertInfo> GetActive();

  /**
   * Resets all alert simulation data.
   */
  static void ResetData();
};

}  // namespace wpi::sim
