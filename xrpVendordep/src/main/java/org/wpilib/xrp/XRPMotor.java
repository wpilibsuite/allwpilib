// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.xrp;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;
import java.util.HashMap;
import java.util.HashSet;

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

  private final SimDouble m_simSpeed;
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
      m_simSpeed = xrpMotorSimDevice.createDouble("speed", Direction.kOutput, 0.0);
    } else {
      m_simInverted = null;
      m_simSpeed = null;
    }
  }

  @Override
  public void set(double speed) {
    if (m_simSpeed != null) {
      boolean invert = false;
      if (m_simInverted != null) {
        invert = m_simInverted.get();
      }

      m_simSpeed.set(invert ? -speed : speed);
    }
  }

  @Override
  public double get() {
    if (m_simSpeed != null) {
      return m_simSpeed.get();
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
    set(0.0);
  }

  @Override
  public void stopMotor() {
    set(0.0);
  }
}
