// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;

public class MockPWMMotorController {
  private double m_power;
  private boolean m_isInverted;

  public void setPower(double power) {
    m_power = m_isInverted ? -power : power;
  }

  public double getPower() {
    return m_power;
  }

  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  public boolean getInverted() {
    return m_isInverted;
  }

  public void disable() {
    m_power = 0;
  }

  public void stopMotor() {
    disable();
  }
}
