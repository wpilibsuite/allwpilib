/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/StringRef.h>

namespace frc {

class ShuffleboardTab;

/**
 * The root of the data placed in Shuffleboard. It contains the tabs, but no
 * data is placed directly in the root.
 *
 * This class is package-private to minimize API surface area.
 */
class ShuffleboardRoot {
 public:
  /**
   * Gets the tab with the given title, creating it if it does not already
   * exist.
   *
   * @param title the title of the tab
   * @return the tab with the given title
   */
  virtual ShuffleboardTab& GetTab(wpi::StringRef title) = 0;

  /**
   * Updates all tabs.
   */
  virtual void Update() = 0;

  /**
   * Enables all widgets in Shuffleboard that offer user control over actuators.
   */
  virtual void EnableActuatorWidgets() = 0;

  /**
   * Disables all widgets in Shuffleboard that offer user control over
   * actuators.
   */
  virtual void DisableActuatorWidgets() = 0;
};

}  // namespace frc
