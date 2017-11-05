/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

/**
 * IterativeBase provides methods that are to be used when code is designed to
 be aware of
 * robot state. These are provided as 'init' functions and 'periodic' functions.
 *
 * Init() functions -- each of the following functions is called once when the
 *					  appropriate mode is entered:

 *   - DisabledInit()   -- called only when first disabled
 *   - AutonomousInit() -- called each and every time autonomous is entered from
 *                         another mode
 *   - TeleopInit()     -- called each and every time teleop is entered from
 *                         another mode
 *   - TestInit()       -- called each and every time test is entered from
 *                         another mode
 *
 * Periodic() functions -- each of these functions is called on an interval:

 *   - RobotPeriodic()
 *   - DisabledPeriodic()
 *   - AutonomousPeriodic()
 *   - TeleopPeriodic()
 *   - TestPeriodic()
 */
class IterativeBase {
 public:
  virtual void DisabledInit() = 0;
  virtual void AutonomousInit() = 0;
  virtual void TeleopInit() = 0;
  virtual void TestInit() = 0;

  virtual void RobotPeriodic() = 0;
  virtual void DisabledPeriodic() = 0;
  virtual void AutonomousPeriodic() = 0;
  virtual void TeleopPeriodic() = 0;
  virtual void TestPeriodic() = 0;
};

}  // namespace frc
