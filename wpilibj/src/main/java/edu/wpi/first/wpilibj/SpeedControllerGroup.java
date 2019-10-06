/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Allows multiple {@link SpeedController} objects to be linked together.
 */
public class SpeedControllerGroup implements SpeedController, Sendable, AutoCloseable {
  private boolean m_isInverted;
  private final SpeedController[] m_speedControllers;
  private static int instances;

  /**
   * Create a new SpeedControllerGroup with the provided SpeedControllers.
   *
   * @param speedControllers The SpeedControllers to add
   */
  @SuppressWarnings("PMD.AvoidArrayLoops")
  public SpeedControllerGroup(SpeedController speedController,
                              SpeedController... speedControllers) {
    m_speedControllers = new SpeedController[speedControllers.length + 1];
    m_speedControllers[0] = speedController;
    SendableRegistry.addChild(this, speedController);
    for (int i = 0; i < speedControllers.length; i++) {
      m_speedControllers[i + 1] = speedControllers[i];
      SendableRegistry.addChild(this, speedControllers[i]);
    }
    instances++;
    SendableRegistry.addLW(this, "tSpeedControllerGroup", instances);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
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
    set(output);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Speed Controller");
    builder.setActuator(true);
    builder.setSafeState(this::stopMotor);
    builder.addDoubleProperty("Value", this::get, this::set);
  }
}
