// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import edu.wpi.first.hal.SimDouble;
import org.wpilib.Servo;

/** Class to control a simulated Servo. */
public class ServoSim {
  private final SimDouble m_simPosition;

  /**
   * Constructor.
   *
   * @param servo The device to simulate
   */
  public ServoSim(Servo servo) {
    this(servo.getChannel());
  }

  /**
   * Constructor.
   *
   * @param channel The channel the servo is attached to.
   */
  public ServoSim(int channel) {
    SimDeviceSim simDevice = new SimDeviceSim("Servo", channel);
    m_simPosition = simDevice.getDouble("Position");
  }

  /**
   * Gets the position set.
   *
   * @return Position
   */
  public double getPosition() {
    return m_simPosition.get();
  }

  /**
   * Gets the angle set.
   *
   * @return Angle
   */
  public double getAngle() {
    return getPosition() * 180.0;
  }
}
