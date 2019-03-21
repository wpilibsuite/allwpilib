/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {
namespace experimental {

/**
 * This interface is a generic measurement source for controllers.
 */
class MeasurementSource {
 public:
  virtual double GetMeasurement() const = 0;
};

}  // namespace experimental
}  // namespace frc
