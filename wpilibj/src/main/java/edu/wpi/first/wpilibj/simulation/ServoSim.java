// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.Servo;

public class ServoSim {
  private final SimDouble m_simPosition;

  public ServoSim(Servo servo) {
    this(servo.getChannel());
  }

  public ServoSim(int channel) {
    SimDeviceSim simDevice = new SimDeviceSim("Servo", channel);
    m_simPosition = simDevice.getDouble("Position");
  }

  public double getPosition() {
    return m_simPosition.get();
  }

  public double getAngle() {
    return getPosition() * 180.0;
  }
}
