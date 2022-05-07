// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.resistance;

import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.PowerDistribution;
import edu.wpi.first.wpilibj.ResistanceCalculator;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * Sample program to demonstrate logging the resistance of a particular PDP/PDH
 * channel and the robot's resistance. The resistance can be calculated by recording
 * the current and voltage of a particular channel and doing a linear regression
 * on that data.
 */
public class Robot extends TimedRobot {
  /**
   * Object representing the PDP or PDH on the robot. The PowerDistribution
   * class implements Sendable and logs the robot resistance. It can also be used
   * to get the current flowing through a particular channel.
   */
  private final PowerDistribution powerDistribution = new PowerDistribution();

  /**
   * The channel whose resistance will be logged in this example.
   */
  private static final int kChannel = 1;

  /**
   * Used to calculate the resistance of channel 1.
   */
  private final ResistanceCalculator m_resistCalc = new ResistanceCalculator();

  /**
   * Used to get the voltage received by the motor plugged into channel 1.
   * It is only a Supplier here because this is an example. In real code, a
   * motor controller on the CAN bus would be used to get the voltage instead.
   */
  private final DoubleSupplier m_chan1Voltage = () -> 1.0;

  @Override
  public void robotInit() {
    // Display the PowerDistribution on the dashboard so that the robot
    // resistance can be seen.
    SmartDashboard.putData(powerDistribution);
  }

  @Override
  public void robotPeriodic() {
    var chan1Current = powerDistribution.getCurrent(kChannel);
    // Get the voltage received by the motor plugged into channel 1. Note that
    // in practice, this voltage would be gotten through a motor controller object.
    var chan1Voltage = m_chan1Voltage.getAsDouble();

    // Calculate and log channel 1's resistance
    SmartDashboard.putNumber(
        "Channel 1 resistance", m_resistCalc.calculate(chan1Current, chan1Voltage));
  }
}
