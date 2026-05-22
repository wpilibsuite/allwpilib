// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;

@SuppressWarnings("removal")
public class MockMotorController implements MotorController {
  private double m_throttle;
  private boolean m_isInverted;

  @Override
  public void setThrottle(double throttle) {
    m_throttle = m_isInverted ? -throttle : throttle;
  }

  @Override
  public double getThrottle() {
    return m_throttle;
  }

  @Override
  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  @Override
  public boolean getInverted() {
    return m_isInverted;
  }

  @Override
  public void disable() {
    m_throttle = 0;
  }
}
