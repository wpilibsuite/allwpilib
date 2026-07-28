// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.hardware.hal.SimDouble;
import org.wpilib.hardware.motor.PWMMotorController;

/** Class to control a simulated PWM motor controller. */
public class PWMMotorControllerSim {
  private final SimDouble m_simThrottle;

  /**
   * Constructor.
   *
   * @param motorctrl The device to simulate
   */
  public PWMMotorControllerSim(PWMMotorController motorctrl) {
    this(motorctrl.getChannel());
  }

  /**
   * Constructor.
   *
   * @param channel The channel the motor controller is attached to.
   */
  public PWMMotorControllerSim(int channel) {
    SimDeviceSim simDevice = new SimDeviceSim("PWMMotorController", channel);
    m_simThrottle = simDevice.getDouble("Throttle");
  }

  /**
   * Gets the motor throttle.
   *
   * @return Throttle
   */
  public double getThrottle() {
    return m_simThrottle.get();
  }
}
