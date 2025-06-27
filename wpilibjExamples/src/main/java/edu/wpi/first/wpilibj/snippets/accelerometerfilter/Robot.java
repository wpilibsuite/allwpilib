// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.accelerometerfilter;

import edu.wpi.first.math.filter.LinearFilter;
import edu.wpi.first.wpilibj.BuiltInAccelerometer;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * Accelerometer filtering snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
public class Robot extends TimedRobot {
  BuiltInAccelerometer m_accelerometer = new BuiltInAccelerometer();
  // Create a LinearFilter that will calculate a moving average of the measured X acceleration over
  // the past 10 iterations of the main loop
  LinearFilter m_xAccelFilter = LinearFilter.movingAverage(10);

  @Override
  public void robotPeriodic() {
    double xAccel = m_accelerometer.getX();
    // Get the filtered X acceleration
    double filteredXAccel = m_xAccelFilter.calculate(xAccel);

    SmartDashboard.putNumber("X Acceleration", xAccel);
    SmartDashboard.putNumber("Filtered X Acceleration", filteredXAccel);
  }
}
