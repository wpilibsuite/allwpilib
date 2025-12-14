// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.xrp;

import java.util.HashMap;
import java.util.HashSet;
import org.wpilib.hardware.hal.SimBoolean;
import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.hardware.hal.SimDevice.Direction;
import org.wpilib.hardware.hal.SimDouble;
import org.wpilib.hardware.motor.MotorController;

/**
 * XRPMotor.
 *
 * <p>A SimDevice based motor controller representing the motors on an XRP robot
 */
@SuppressWarnings("removal")
public class XRPMotor implements MotorController {
  private static HashMap<Integer, String> s_simDeviceNameMap = new HashMap<>();
  private static HashSet<Integer> s_registeredDevices = new HashSet<>();

  private static void checkDeviceAllocation(int deviceNum) {
    if (!s_simDeviceNameMap.containsKey(deviceNum)) {
      throw new IllegalArgumentException("Invalid XRPMotor device number. Should be 0-3");
    }

    if (s_registeredDevices.contains(deviceNum)) {
      throw new IllegalArgumentException("XRPMotor " + deviceNum + " already allocated");
    }

    s_registeredDevices.add(deviceNum);
  }

  static {
    s_simDeviceNameMap.put(0, "motorL");
    s_simDeviceNameMap.put(1, "motorR");
    s_simDeviceNameMap.put(2, "motor3");
    s_simDeviceNameMap.put(3, "motor4");
  }

  private final SimDouble m_simVelocity;
  private final SimBoolean m_simInverted;

  /**
   * Constructs an XRPMotor.
   *
   * @param deviceNum the motor channel
   */
  public XRPMotor(int deviceNum) {
    checkDeviceAllocation(deviceNum);

    // We want this to appear on the WS messages as type: "XRPMotor", device: <motor name>
    String simDeviceName = "XRPMotor:" + s_simDeviceNameMap.get(deviceNum);
    SimDevice xrpMotorSimDevice = SimDevice.create(simDeviceName);

    if (xrpMotorSimDevice != null) {
      xrpMotorSimDevice.createBoolean("init", Direction.kOutput, true);
      m_simInverted = xrpMotorSimDevice.createBoolean("inverted", Direction.kInput, false);
      m_simVelocity = xrpMotorSimDevice.createDouble("velocity", Direction.kOutput, 0.0);
    } else {
      m_simInverted = null;
      m_simVelocity = null;
    }
  }

  @Override
  public void setDutyCycle(double velocity) {
    if (m_simVelocity != null) {
      boolean invert = false;
      if (m_simInverted != null) {
        invert = m_simInverted.get();
      }

      m_simVelocity.set(invert ? -velocity : velocity);
    }
  }

  @Override
  public double getDutyCycle() {
    if (m_simVelocity != null) {
      return m_simVelocity.get();
    }

    return 0.0;
  }

  @Override
  public void setInverted(boolean isInverted) {
    if (m_simInverted != null) {
      m_simInverted.set(isInverted);
    }
  }

  @Override
  public boolean getInverted() {
    return m_simInverted != null && m_simInverted.get();
  }

  @Override
  public void disable() {
    setDutyCycle(0.0);
  }
}
