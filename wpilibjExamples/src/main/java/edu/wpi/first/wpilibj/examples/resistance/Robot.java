// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.resistance;

import edu.wpi.first.wpilibj.PowerDistribution;
import edu.wpi.first.wpilibj.ResistanceCalculator;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * Sample program to demonstrate logging the resistance of a particular PDP/PDH channel and the
 * robot's resistance. The resistance can be calculated by recording the current and voltage of a
 * particular channel and doing a linear regression on that data.
 */
public class Robot extends TimedRobot {
  /** The channel on the PDP whose resistance will be logged in this example. */
  private static final int kChannel = 1;

  /**
   * Object representing the PDP or PDH on the robot. The PowerDistribution class implements
   * Sendable and logs the robot resistance. It can also be used to get the current flowing through
   * a particular channel.
   */
  private final PowerDistribution m_powerDistribution = new PowerDistribution();

  /** Used to calculate the resistance of channel 1. */
  private final ResistanceCalculator m_resistCalc1 = new ResistanceCalculator();

  /** The motor plugged into channel 1. */
  private final MotorController m_motor = new PWMSparkMax(0);

  @Override
  public void robotInit() {
    // Display the PowerDistribution on the dashboard so that the robot
    // resistance can be seen.
    SmartDashboard.putData(m_powerDistribution);
  }

  @Override
  public void robotPeriodic() {
    // Get the current for channel 1
    var chan1Current = m_powerDistribution.getCurrent(kChannel);

    // Get the voltage given to the motor plugged into channel 1
    var chan1Voltage = m_motor.get() * RobotController.getBatteryVoltage();

    // Log the resistance
    SmartDashboard.putNumber(
        "Channel 1 resistance", m_resistCalc1.calculate(chan1Current, chan1Voltage).getAsDouble());
    SmartDashboard.putNumber("Robot resistance", m_powerDistribution.getTotalResistance());
  }
}
