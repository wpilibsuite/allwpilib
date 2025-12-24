// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;

public class MockPWMMotorController {
  private double m_dutyCycle;
  private boolean m_isInverted;

  public void setDutyCycle(double dutyCycle) {
    m_dutyCycle = m_isInverted ? -dutyCycle : dutyCycle;
  }

  public double getDutyCycle() {
    return m_dutyCycle;
  }

  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  public boolean getInverted() {
    return m_isInverted;
  }

  public void disable() {
    m_dutyCycle = 0;
  }

  public void stopMotor() {
    disable();
  }
}
