// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * SharpIR analog distance sensor class. These distance measuring sensors output an analog voltage
 * corresponding to the detection distance.
 *
 * <p>Teams are advised that the case of these sensors are conductive and grounded, so they should
 * not be mounted on a metallic surface on an FRC robot.
 */
@SuppressWarnings("MethodName")
public class SharpIR implements Sendable, AutoCloseable {
  private AnalogInput m_sensor;

  private SimDevice m_simDevice;
  private SimDouble m_simRange;

  private final double m_A;
  private final double m_B;
  private final double m_minCM;
  private final double m_maxCM;

  /**
   * Sharp GP2Y0A02YK0F is an analog IR sensor capable of measuring distances from 20cm to 150cm.
   *
   * @param channel Analog input channel the sensor is connected to
   * @return sensor object
   */
  public static SharpIR GP2Y0A02YK0F(int channel) {
    return new SharpIR(channel, 62.28, -1.092, 20, 150.0);
  }

  /**
   * Sharp GP2Y0A21YK0F is an analog IR sensor capable of measuring distances from 10cm to 80cm.
   *
   * @param channel Analog input channel the sensor is connected to
   * @return sensor object
   */
  public static SharpIR GP2Y0A21YK0F(int channel) {
    return new SharpIR(channel, 26.449, -1.226, 10.0, 80.0);
  }

  /**
   * Sharp GP2Y0A41SK0F is an analog IR sensor capable of measuring distances from 4cm to 30cm.
   *
   * @param channel Analog input channel the sensor is connected to
   * @return sensor object
   */
  public static SharpIR GP2Y0A41SK0F(int channel) {
    return new SharpIR(channel, 12.354, -1.07, 4.0, 30.0);
  }

  /**
   * Sharp GP2Y0A51SK0F is an analog IR sensor capable of measuring distances from 2cm to 15cm.
   *
   * @param channel Analog input channel the sensor is connected to
   * @return sensor object
   */
  public static SharpIR GP2Y0A51SK0F(int channel) {
    return new SharpIR(channel, 5.2819, -1.161, 2.0, 15.0);
  }

  /**
   * Manually construct a SharpIR object. The distance is computed using this formula: A*v ^ B.
   * Prefer to use one of the static factories to create this device instead.
   *
   * @param channel AnalogInput channel
   * @param a Constant A
   * @param b Constant B
   * @param minCM Minimum distance to report in centimeters
   * @param maxCM Maximum distance to report in centimeters
   */
  @SuppressWarnings("this-escape")
  public SharpIR(int channel, double a, double b, double minCM, double maxCM) {
    m_sensor = new AnalogInput(channel);

    m_A = a;
    m_B = b;
    m_minCM = minCM;
    m_maxCM = maxCM;

    SendableRegistry.addLW(this, "SharpIR", channel);

    m_simDevice = SimDevice.create("SharpIR", m_sensor.getChannel());
    if (m_simDevice != null) {
      m_simRange = m_simDevice.createDouble("Range (cm)", Direction.kInput, 0.0);
      m_sensor.setSimDevice(m_simDevice);
    }
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
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
   * Get the range in inches from the distance sensor.
   *
   * @return range in inches of the target returned by the sensor
   */
  public double getRangeInches() {
    return getRangeCM() / 2.54;
  }

  /**
   * Get the range in centimeters from the distance sensor.
   *
   * @return range in centimeters of the target returned by the sensor
   */
  public double getRangeCM() {
    double distance;

    if (m_simRange != null) {
      distance = m_simRange.get();
    } else {
      // Don't allow zero/negative values
      var v = Math.max(m_sensor.getVoltage(), 0.00001);
      distance = m_A * Math.pow(v, m_B);
    }

    // Always constrain output
    return Math.max(Math.min(distance, m_maxCM), m_minCM);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Ultrasonic");
    builder.addDoubleProperty("Value", this::getRangeInches, null);
  }
}
