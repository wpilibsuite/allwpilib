// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.motorcontrol.PWMMotorController;

/** Class to control a simulated PWM motor controller. */
public class PWMMotorControllerSim {
  private final SimDouble m_simSpeed;

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
    m_simSpeed = simDevice.getDouble("Speed");
  }

  /**
   * Gets the motor speed set.
   *
   * @return Speed
   */
  public double getSpeed() {
    return m_simSpeed.get();
  }
}
