/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * Allows multiple {@link SpeedController} objects to be linked together.
 */
public class SpeedControllerGroup extends SendableBase implements SpeedController {
  private boolean m_isInverted = false;
  private final SpeedController[] m_speedControllers;
  private static int instances = 0;

  /**
   * Create a new SpeedControllerGroup with the provided SpeedControllers.
   *
   * @param speedControllers The SpeedControllers to add
   */
  public SpeedControllerGroup(SpeedController speedController,
                              SpeedController... speedControllers) {
    m_speedControllers = new SpeedController[speedControllers.length + 1];
    m_speedControllers[0] = speedController;
    addChild(speedController);
    for (int i = 0; i < speedControllers.length; i++) {
      m_speedControllers[i + 1] = speedControllers[i];
      addChild(speedControllers[i]);
    }
    instances++;
    setName("SpeedControllerGroup", instances);
  }

  @Override
  public void set(double speed) {
    for (SpeedController speedController : m_speedControllers) {
      speedController.set(m_isInverted ? -speed : speed);
    }
  }

  @Override
  public double get() {
    if (m_speedControllers.length > 0) {
      return m_speedControllers[0].get() * (m_isInverted ? -1 : 1);
    }
    return 0.0;
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

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Speed Controller");
    builder.setSafeState(this::stopMotor);
    builder.addDoubleProperty("Value", this::get, this::set);
  }
}
