/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Allows multiple {@link SpeedController} objects to be linked together.
 */
public class SpeedControllerGroup implements SpeedController {

  private boolean m_isInverted = false;
  private final SpeedController[] m_speedControllers;

  /**
   * Create a new SpeedControllerGroup with the provided SpeedControllers.
   *
   * @param speedControllers The SpeedControllers to add
   */
  public SpeedControllerGroup(SpeedController speedController,
                              SpeedController... speedControllers) {
    m_speedControllers = new SpeedController[speedControllers.length + 1];
    m_speedControllers[0] = speedController;
    for (int i = 0; i < m_speedControllers.length; i++) {
      m_speedControllers[i + 1] = speedControllers[i];
    }
  }

  @Override
  public double get() {
    if (m_speedControllers.length > 0) {
      return m_speedControllers[0].get();
    }
    return 0.0;
  }

  @Override
  public void set(double speed) {
    for (SpeedController speedController : m_speedControllers) {
      speedController.set(m_isInverted ? -speed : speed);
    }
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
    for (SpeedController speedController : m_speedControllers) {
      speedController.disable();
    }
  }

  @Override
  public void stopMotor() {
    for (SpeedController speedController : m_speedControllers) {
      speedController.stopMotor();
    }
  }

  @Override
  public void pidWrite(double output) {
    for (SpeedController speedController : m_speedControllers) {
      speedController.pidWrite(output);
    }
  }
}
