// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;

public class MockPWMMotorController {
  private double m_throttle;
  private boolean m_isInverted;

  public void setThrottle(double throttle) {
    m_throttle = m_isInverted ? -throttle : throttle;
  }

  public double getThrottle() {
    return m_throttle;
  }

  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  public boolean getInverted() {
    return m_isInverted;
  }

  public void disable() {
    m_throttle = 0;
  }

  public void stopMotor() {
    disable();
  }
}
