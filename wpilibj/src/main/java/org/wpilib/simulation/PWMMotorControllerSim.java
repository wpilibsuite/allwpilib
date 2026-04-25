// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.hardware.hal.SimDouble;
import org.wpilib.hardware.motor.PWMMotorController;

/** Class to control a simulated PWM motor controller. */
public class PWMMotorControllerSim {
  private final SimDouble m_simPower;

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
    m_simPower = simDevice.getDouble("Power");
  }

  /**
   * Gets the motor power.
   *
   * @return Power
   */
  public double getPower() {
    return m_simPower.get();
  }
}
