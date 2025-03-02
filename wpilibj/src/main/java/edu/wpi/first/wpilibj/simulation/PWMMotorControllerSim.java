// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.motorcontrol.PWMMotorController;

public class PWMMotorControllerSim {
  private final SimDouble m_simSpeed;

  public PWMMotorControllerSim(PWMMotorController motorctrl) {
    this(motorctrl.getName(), motorctrl.getChannel());
  }

  public PWMMotorControllerSim(String name, int channel) {
    SimDeviceSim simDevice = new SimDeviceSim(name, channel);
    m_simSpeed = simDevice.getDouble("Speed");
  }

  public double getSpeed() {
    return m_simSpeed.get();
  }


}
