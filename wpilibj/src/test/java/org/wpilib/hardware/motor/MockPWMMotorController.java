// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;

public class MockPWMMotorController {
  private double m_speed;
  private boolean m_isInverted;

  public void set(double speed) {
    m_speed = m_isInverted ? -speed : speed;
  }

  public double get() {
    return m_speed;
  }

  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  public boolean getInverted() {
    return m_isInverted;
  }

  public void disable() {
    m_speed = 0;
  }

  public void stopMotor() {
    disable();
  }
}
