/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "Counter.h"

namespace frc {

/**
 * Alias for counter class.
 *
 * Implements the gear tooth sensor supplied by FIRST. Currently there is no
 * reverse sensing on the gear tooth sensor, but in future versions we might
 * implement the necessary timing in the FPGA to sense direction.
 */
class GearTooth : public Counter {
 public:
  // 55 uSec for threshold
  static constexpr double kGearToothThreshold = 55e-6;

  explicit GearTooth(int channel, bool directionSensitive = false);
  explicit GearTooth(DigitalSource* source, bool directionSensitive = false);
  explicit GearTooth(std::shared_ptr<DigitalSource> source,
                     bool directionSensitive = false);

  void EnableDirectionSensing(bool directionSensitive);

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
