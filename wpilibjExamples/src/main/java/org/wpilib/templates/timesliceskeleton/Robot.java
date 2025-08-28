// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.templates.timesliceskeleton;

import org.wpilib.TimesliceRobot;

/**
 * The methods in this class are called automatically corresponding to each mode, as described in
 * the TimesliceRobot documentation. If you change the name of this class or the package after
 * creating this project, you must also update the Main.java file in the project.
 */
public class Robot extends TimesliceRobot {
  /** Robot constructor. */
  public Robot() {
    // Run robot periodic() functions for 5 ms, and run controllers every 10 ms
    super(0.005, 0.01);

    // Runs for 2 ms after robot periodic functions
    schedule(() -> {}, 0.002);

    // Runs for 2 ms after first controller function
    schedule(() -> {}, 0.002);

    // Total usage:
    // 5 ms (robot) + 2 ms (controller 1) + 2 ms (controller 2) = 9 ms
    // 9 ms / 10 ms -> 90% allocated
  }

  @Override
  public void robotPeriodic() {}

  @Override
  public void autonomousInit() {}

  @Override
  public void autonomousPeriodic() {}

  @Override
  public void teleopInit() {}

  @Override
  public void teleopPeriodic() {}

  @Override
  public void disabledInit() {}

  @Override
  public void disabledPeriodic() {}

  @Override
  public void testInit() {}

  @Override
  public void testPeriodic() {}
}
