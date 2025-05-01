// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.hal.SimEnum;
import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.DoubleTopic;
import edu.wpi.first.networktables.NTSendable;
import edu.wpi.first.networktables.NTSendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/** ADXL345 I2C Accelerometer. */
@SuppressWarnings("TypeName")
public class ADXL345_I2C implements NTSendable, AutoCloseable {
  /** Default I2C device address. */
  public static final byte ADDRESS = 0x1D;

  private static final byte POWER_CTL_REGISTER = 0x2D;
  private static final byte DATA_FORMAT_REGISTER = 0x31;
  private static final byte DATA_REGISTER = 0x32;
  private static final double GS_PER_LSB = 0.00390625;
  // private static final byte POWER_CTL_LINK = 0x20;
  // private static final byte POWER_CTL_AUTO_SLEEP = 0x10;
  private static final byte POWER_CTL_MEASURE = 0x08;
  // private static final byte POWER_CTL_SLEEP = 0x04;

  // private static final byte DATA_FORMAT_SELF_TEST = (byte) 0x80;
  // private static final byte DATA_FORMAT_SPI = 0x40;
  // private static final byte DATA_FORMAT_INT_INVERT = 0x20;
  private static final byte DATA_FORMAT_FULL_RES = 0x08;

  // private static final byte DATA_FORMAT_JUSTIFY = 0x04;

  /** Accelerometer range. */
  public enum Range {
    /** 2 Gs max. */
    TWO_G,
    /** 4 Gs max. */
    FOUR_G,
    /** 8 Gs max. */
    EIGHT_G,
    /** 16 Gs max. */
    SIXTEEN_G
  }

  /** Accelerometer axes. */
  public enum Axes {
    /** X axis. */
    X((byte) 0x00),
    /** Y axis. */
    Y((byte) 0x02),
    /** Z axis. */
    Z((byte) 0x04);

    /** The integer value representing this enumeration. */
    public final byte value;

    Axes(byte value) {
      this.value = value;
    }
  }

  /** Container type for accelerations from all axes. */
  @SuppressWarnings("MemberName")
  public static class AllAxes {
    /** Acceleration along the X axis in g-forces. */
    public double XAxis;

    /** Acceleration along the Y axis in g-forces. */
    public double YAxis;

    /** Acceleration along the Z axis in g-forces. */
    public double ZAxis;

    /** Default constructor. */
    public AllAxes() {}
  }

  private I2C m_i2c;

  private SimDevice m_simDevice;
  private SimEnum m_simRange;
  private SimDouble m_simX;
  private SimDouble m_simY;
  private SimDouble m_simZ;

  /**
   * Constructs the ADXL345 Accelerometer with I2C address 0x1D.
   *
   * @param port The I2C port the accelerometer is attached to
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  public ADXL345_I2C(I2C.Port port, Range range) {
    this(port, range, ADDRESS);
  }

  /**
   * Constructs the ADXL345 Accelerometer over I2C.
   *
   * @param port The I2C port the accelerometer is attached to
   * @param range The range (+ or -) that the accelerometer will measure.
   * @param deviceAddress I2C address of the accelerometer (0x1D or 0x53)
   */
  @SuppressWarnings("this-escape")
  public ADXL345_I2C(I2C.Port port, Range range, int deviceAddress) {
    m_i2c = new I2C(port, deviceAddress);

    // simulation
    m_simDevice = SimDevice.create("Accel:ADXL345_I2C", port.value, deviceAddress);
    if (m_simDevice != null) {
      m_simRange =
          m_simDevice.createEnumDouble(
              "range",
              SimDevice.Direction.OUTPUT,
              new String[] {"2G", "4G", "8G", "16G"},
              new double[] {2.0, 4.0, 8.0, 16.0},
              0);
      m_simX = m_simDevice.createDouble("x", SimDevice.Direction.INPUT, 0.0);
      m_simY = m_simDevice.createDouble("y", SimDevice.Direction.INPUT, 0.0);
      m_simZ = m_simDevice.createDouble("z", SimDevice.Direction.INPUT, 0.0);
    }

    // Turn on the measurements
    m_i2c.write(POWER_CTL_REGISTER, POWER_CTL_MEASURE);

    setRange(range);

    HAL.reportUsage("I2C[" + port.value + "][" + deviceAddress + "]", "ADXL345");
    SendableRegistry.add(this, "ADXL345_I2C", port.value);
  }

  /**
   * Returns the I2C port.
   *
   * @return The I2C port.
   */
  public int getPort() {
    return m_i2c.getPort();
  }

  /**
   * Returns the I2C device address.
   *
   * @return The I2C device address.
   */
  public int getDeviceAddress() {
    return m_i2c.getDeviceAddress();
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    if (m_i2c != null) {
      m_i2c.close();
      m_i2c = null;
    }
    if (m_simDevice != null) {
      m_simDevice.close();
      m_simDevice = null;
    }
  }

  /**
   * Set the measuring range of the accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the accelerometer will
   *     measure.
   */
  public final void setRange(Range range) {
    final byte value =
        switch (range) {
          case TWO_G -> 0;
          case FOUR_G -> 1;
          case EIGHT_G -> 2;
          case SIXTEEN_G -> 3;
        };

    // Specify the data format to read
    m_i2c.write(DATA_FORMAT_REGISTER, DATA_FORMAT_FULL_RES | value);

    if (m_simRange != null) {
      m_simRange.set(value);
    }
  }

  /**
   * Returns the acceleration along the X axis in g-forces.
   *
   * @return The acceleration along the X axis in g-forces.
   */
  public double getX() {
    return getAcceleration(Axes.X);
  }

  /**
   * Returns the acceleration along the Y axis in g-forces.
   *
   * @return The acceleration along the Y axis in g-forces.
   */
  public double getY() {
    return getAcceleration(Axes.Y);
  }

  /**
   * Returns the acceleration along the Z axis in g-forces.
   *
   * @return The acceleration along the Z axis in g-forces.
   */
  public double getZ() {
    return getAcceleration(Axes.Z);
  }

  /**
   * Get the acceleration of one axis in Gs.
   *
   * @param axis The axis to read from.
   * @return Acceleration of the ADXL345 in Gs.
   */
  public double getAcceleration(Axes axis) {
    if (axis == Axes.X && m_simX != null) {
      return m_simX.get();
    }
    if (axis == Axes.Y && m_simY != null) {
      return m_simY.get();
    }
    if (axis == Axes.Z && m_simZ != null) {
      return m_simZ.get();
    }
    ByteBuffer rawAccel = ByteBuffer.allocate(2);
    m_i2c.read(DATA_REGISTER + axis.value, 2, rawAccel);

    // Sensor is little endian... swap bytes
    rawAccel.order(ByteOrder.LITTLE_ENDIAN);
    return rawAccel.getShort(0) * GS_PER_LSB;
  }

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the ADXL345 in Gs.
   */
  public AllAxes getAccelerations() {
    AllAxes data = new AllAxes();
    if (m_simX != null && m_simY != null && m_simZ != null) {
      data.XAxis = m_simX.get();
      data.YAxis = m_simY.get();
      data.ZAxis = m_simZ.get();
      return data;
    }
    ByteBuffer rawData = ByteBuffer.allocate(6);
    m_i2c.read(DATA_REGISTER, 6, rawData);

    // Sensor is little endian... swap bytes
    rawData.order(ByteOrder.LITTLE_ENDIAN);
    data.XAxis = rawData.getShort(0) * GS_PER_LSB;
    data.YAxis = rawData.getShort(2) * GS_PER_LSB;
    data.ZAxis = rawData.getShort(4) * GS_PER_LSB;
    return data;
  }

  @Override
  public void initSendable(NTSendableBuilder builder) {
    builder.setSmartDashboardType("3AxisAccelerometer");
    DoublePublisher pubX = new DoubleTopic(builder.getTopic("X")).publish();
    DoublePublisher pubY = new DoubleTopic(builder.getTopic("Y")).publish();
    DoublePublisher pubZ = new DoubleTopic(builder.getTopic("Z")).publish();
    builder.addCloseable(pubX);
    builder.addCloseable(pubY);
    builder.addCloseable(pubZ);
    builder.setUpdateTable(
        () -> {
          AllAxes data = getAccelerations();
          pubX.set(data.XAxis);
          pubY.set(data.YAxis);
          pubZ.set(data.ZAxis);
        });
  }
}
