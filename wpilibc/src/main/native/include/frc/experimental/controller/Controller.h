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
 * Base class for Controllers.
 *
 * Common base class for controllers. Controllers run control loops, the most
 * common are PID controllers and their variants, but this includes anything
 * that is controlling an actuator in a separate thread.
 */
class Controller {
 public:
  explicit Controller(double period);

  virtual ~Controller() = default;

  Controller(Controller&&) = default;
  Controller& operator=(Controller&&) = default;

  double GetPeriod() const;

  /**
   * Read the input, calculate the output accordingly, and write to the output.
   *
   * @return The controller output.
   */
  virtual double Update() = 0;

 private:
  double m_period;
};

}  // namespace experimental
}  // namespace frc
