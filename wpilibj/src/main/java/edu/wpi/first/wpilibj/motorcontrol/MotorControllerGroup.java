// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.telemetry.TelemetryLoggable;
import edu.wpi.first.telemetry.TelemetryTable;
import java.util.Arrays;

/**
 * Allows multiple {@link MotorController} objects to be linked together.
 *
 * @deprecated Use {@link PWMMotorController#addFollower(PWMMotorController)} or if using CAN motor
 *     controllers, use their method of following.
 */
@Deprecated(forRemoval = true, since = "2024")
public class MotorControllerGroup implements MotorController, TelemetryLoggable, AutoCloseable {
  private boolean m_isInverted;
  private final MotorController[] m_motorControllers;

  /**
   * Create a new MotorControllerGroup with the provided MotorControllers.
   *
   * @param motorController The first MotorController to add
   * @param motorControllers The MotorControllers to add
   */
  public MotorControllerGroup(
      MotorController motorController, MotorController... motorControllers) {
    m_motorControllers = new MotorController[motorControllers.length + 1];
    m_motorControllers[0] = motorController;
    System.arraycopy(motorControllers, 0, m_motorControllers, 1, motorControllers.length);
  }

  /**
   * Create a new MotorControllerGroup with the provided MotorControllers.
   *
   * @param motorControllers The MotorControllers to add.
   */
  public MotorControllerGroup(MotorController[] motorControllers) {
    m_motorControllers = Arrays.copyOf(motorControllers, motorControllers.length);
  }

  @Override
  public void close() {}

  @Override
  public void set(double speed) {
    for (MotorController motorController : m_motorControllers) {
      motorController.set(m_isInverted ? -speed : speed);
    }
  }

  @Override
  public void setVoltage(double outputVolts) {
    for (MotorController motorController : m_motorControllers) {
      motorController.setVoltage(m_isInverted ? -outputVolts : outputVolts);
    }
  }

  @Override
  public double get() {
    if (m_motorControllers.length > 0) {
      return m_motorControllers[0].get() * (m_isInverted ? -1 : 1);
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
    for (MotorController motorController : m_motorControllers) {
      motorController.disable();
    }
  }

  @Override
  public void stopMotor() {
    for (MotorController motorController : m_motorControllers) {
      motorController.stopMotor();
    }
  }

  @Override
  public void updateTelemetry(TelemetryTable table) {
    table.log("Value", get());
  }

  @Override
  public String getTelemetryType() {
    return "Motor Controller";
  }
}
