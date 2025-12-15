// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.range;

import org.wpilib.hardware.discrete.AnalogInput;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.hardware.hal.SimDevice.Direction;
import org.wpilib.hardware.hal.SimDouble;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;

/**
 * SharpIR analog distance sensor class. These distance measuring sensors output an analog voltage
 * corresponding to the detection distance.
 *
 * <p>Teams are advised that the case of these sensors are conductive and grounded, so they should
 * not be mounted on a metallic surface on an FRC robot.
 */
@SuppressWarnings("MethodName")
public class SharpIR implements TelemetryLoggable, AutoCloseable {
  private AnalogInput m_sensor;

  private SimDevice m_simDevice;
  private SimDouble m_simRange;

  private final double m_A;
  private final double m_B;
  private final double m_min; // m
  private final double m_max; // m

  /**
   * Sharp GP2Y0A02YK0F is an analog IR sensor capable of measuring distances from 20 cm to 150 cm.
   *
   * @param channel Analog input channel the sensor is connected to
   * @return sensor object
   */
  public static SharpIR GP2Y0A02YK0F(int channel) {
    return new SharpIR(channel, 62.28, -1.092, 0.2, 1.5);
  }

  /**
   * Sharp GP2Y0A21YK0F is an analog IR sensor capable of measuring distances from 10 cm to 80 cm.
   *
   * @param channel Analog input channel the sensor is connected to
   * @return sensor object
   */
  public static SharpIR GP2Y0A21YK0F(int channel) {
    return new SharpIR(channel, 26.449, -1.226, 0.1, 0.8);
  }

  /**
   * Sharp GP2Y0A41SK0F is an analog IR sensor capable of measuring distances from 4 cm to 30 cm.
   *
   * @param channel Analog input channel the sensor is connected to
   * @return sensor object
   */
  public static SharpIR GP2Y0A41SK0F(int channel) {
    return new SharpIR(channel, 12.354, -1.07, 0.04, 0.3);
  }

  /**
   * Sharp GP2Y0A51SK0F is an analog IR sensor capable of measuring distances from 2 cm to 15 cm.
   *
   * @param channel Analog input channel the sensor is connected to
   * @return sensor object
   */
  public static SharpIR GP2Y0A51SK0F(int channel) {
    return new SharpIR(channel, 5.2819, -1.161, 0.02, 0.15);
  }

  /**
   * Manually construct a SharpIR object. The distance is computed using this formula: A*v ^ B.
   * Prefer to use one of the static factories to create this device instead.
   *
   * @param channel AnalogInput channel
   * @param a Constant A
   * @param b Constant B
   * @param min Minimum distance to report in meters
   * @param max Maximum distance to report in meters
   */
  public SharpIR(int channel, double a, double b, double min, double max) {
    m_sensor = new AnalogInput(channel);

    m_A = a;
    m_B = b;
    m_min = min;
    m_max = max;

    HAL.reportUsage("IO", channel, "SharpIR");

    m_simDevice = SimDevice.create("SharpIR", m_sensor.getChannel());
    if (m_simDevice != null) {
      m_simRange = m_simDevice.createDouble("Range (m)", Direction.kInput, 0.0);
      m_sensor.setSimDevice(m_simDevice);
    }
  }

  @Override
  public void close() {
    m_sensor.close();
    m_sensor = null;

    if (m_simDevice != null) {
      m_simDevice.close();
      m_simDevice = null;
      m_simRange = null;
    }
  }

  /**
   * Get the analog input channel number.
   *
   * @return analog input channel
   */
  public int getChannel() {
    return m_sensor.getChannel();
  }

  /**
   * Get the range in meters from the distance sensor.
   *
   * @return range in meters of the target returned by the sensor
   */
  public double getRange() {
    if (m_simRange != null) {
      return Math.clamp(m_simRange.get(), m_min, m_max);
    } else {
      // Don't allow zero/negative values
      var v = Math.max(m_sensor.getVoltage(), 0.00001);

      return Math.clamp(m_A * Math.pow(v, m_B) * 1e-2, m_min, m_max);
    }
  }

  @Override
  public void logTo(TelemetryTable table) {
    table.log("Value", getRange());
  }

  @Override
  public String getTelemetryType() {
    return "Ultrasonic";
  }
}
