/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
