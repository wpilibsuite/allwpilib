// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.romi;

import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.hardware.hal.SimDevice.Direction;
import org.wpilib.hardware.hal.SimDouble;
import org.wpilib.hardware.motor.MotorController;

/**
 * RomiMotor.
 *
 * <p>A SimDevice based motor controller representing the motors on a Romi robot
 */
public class RomiMotor implements MotorController {
  private final SimDouble m_simSpeed;
  private boolean m_inverted;

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the RomiMotor is attached to. 0 is the left motor, 1 is the
   *     right.
   */
  public RomiMotor(int channel) {
    OnBoardIO.allocatePWM(channel);

    // We want this to appear on the WS messages as type: "PWM", device: <channel>
    String simDeviceName = "PWM:" + channel;
    SimDevice romiMotorSimDevice = SimDevice.create(simDeviceName);

    if (romiMotorSimDevice != null) {
      romiMotorSimDevice.createBoolean("init", Direction.OUTPUT, true);
      m_simSpeed = romiMotorSimDevice.createDouble("speed", Direction.OUTPUT, 0.0);
    } else {
      m_simSpeed = null;
    }
  }

  @Override
  public void setThrottle(double throttle) {
    if (m_simSpeed != null) {
      m_simSpeed.set(m_inverted ? -throttle : throttle);
    }
  }

  @Override
  public double getThrottle() {
    if (m_simSpeed != null) {
      return m_simSpeed.get();
    }

    return 0.0;
  }

  @Override
  public void setInverted(boolean isInverted) {
    m_inverted = isInverted;
  }

  @Override
  public boolean getInverted() {
    return m_inverted;
  }

  @Override
  public void disable() {
    setThrottle(0.0);
  }
}
