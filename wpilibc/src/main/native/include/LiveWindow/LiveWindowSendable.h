/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "SmartDashboard/Sendable.h"

namespace frc {

/**
 * Live Window Sendable is a special type of object sendable to the live window.
 */
class LiveWindowSendable : public Sendable {
 public:
  /**
   * Update the table for this sendable object with the latest
   * values.
   */
  virtual void UpdateTable() = 0;

  /**
   * Start having this sendable object automatically respond to
   * value changes reflect the value on the table.
   */
  virtual void StartLiveWindowMode() = 0;

  /**
   * Stop having this sendable object automatically respond to value
   * changes.
   */
  virtual void StopLiveWindowMode() = 0;
};

}  // namespace frc
