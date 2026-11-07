// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.romi;

import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.hardware.hal.SimDevice.Direction;
import org.wpilib.hardware.hal.SimDouble;

/**
 * RomiServo.
 *
 * <p>A SimDevice based servo
 */
public class RomiServo {
  private final SimDouble m_simPosition;

  /**
   * Constructs a RomiServo.
   *
   * @param channel the PWM channel the servo is attached to.
   */
  public RomiServo(int channel) {
    OnBoardIO.allocatePWM(channel);

    // We want this to appear on WS as type: "PWM", device: <channel>
    String simDeviceName = "PWM:" + channel;
    SimDevice romiServoSimDevice = SimDevice.create(simDeviceName);

    if (romiServoSimDevice != null) {
      romiServoSimDevice.createBoolean("init", Direction.OUTPUT, true);
      // This should mimic PWM position [0.0, 1.0]
      m_simPosition = romiServoSimDevice.createDouble("position", Direction.OUTPUT, 0.5);
    } else {
      m_simPosition = null;
    }
  }

  /**
   * Set the servo angle.
   *
   * @param angleDegrees Desired angle in degrees
   */
  public void setAngle(double angleDegrees) {
    if (angleDegrees < 0.0) {
      angleDegrees = 0.0;
    }

    if (angleDegrees > 180.0) {
      angleDegrees = 180.0;
    }

    double pos = angleDegrees / 180.0;

    if (m_simPosition != null) {
      m_simPosition.set(pos);
    }
  }

  /**
   * Get the servo angle.
   *
   * @return Current servo angle
   */
  public double getAngle() {
    if (m_simPosition != null) {
      return m_simPosition.get() * 180.0;
    }

    return 90.0;
  }

  /**
   * Set the servo position.
   *
   * @param position Desired position (Between 0.0 and 1.0)
   */
  public void setPosition(double position) {
    if (position < 0.0) {
      position = 0.0;
    }

    if (position > 1.0) {
      position = 1.0;
    }

    if (m_simPosition != null) {
      m_simPosition.set(position);
    }
  }

  /**
   * Get the servo position.
   *
   * @return Current servo position
   */
  public double getPosition() {
    if (m_simPosition != null) {
      return m_simPosition.get();
    }

    return 0.5;
  }
}
