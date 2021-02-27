// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

public class MockSpeedController implements SpeedController {
  private double m_speed;
  private boolean m_isInverted;

  @Override
  public void set(double speed) {
    m_speed = m_isInverted ? -speed : speed;
  }

  @Override
  public double get() {
    return m_speed;
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
    m_speed = 0;
  }

  @Override
  public void stopMotor() {
    disable();
  }

  @Override
  public void pidWrite(double output) {
    set(output);
  }
}
