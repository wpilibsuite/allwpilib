// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.accelerometer;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.wpilib.hardware.bus.I2C;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.hardware.hal.SimDouble;
import org.wpilib.hardware.hal.SimEnum;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;

/** ADXL345 I2C Accelerometer. */
@SuppressWarnings("TypeName")
public class ADXL345_I2C implements TelemetryLoggable, AutoCloseable {
  /** Default I2C device address. */
  public static final byte DEFAULT_ADDRESS = 0x1D;

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

  /** Accelerometer axes. */
  public enum Axis {
    /** X axis. */
    X((byte) 0x00),
    /** Y axis. */
    Y((byte) 0x02),
    /** Z axis. */
    Z((byte) 0x04);

    /** The integer value representing this enumeration. */
    public final byte value;

    Axis(byte value) {
      this.value = value;
    }
  }

  /**
   * Container type for accelerations from all axes.
   *
   * @param x x-axis value
   * @param y y-axis value
   * @param z z-axis value
   */
  public record AllAxes(double x, double y, double z) implements StructSerializable {
    /** Struct implementation for AllAxes. */
    public static class AllAxesStruct implements Struct<AllAxes> {
      /** Constructor. */
      public AllAxesStruct() {}

      @Override
      public Class<AllAxes> getTypeClass() {
        return AllAxes.class;
      }

      @Override
      public String getTypeName() {
        return "ADXL345_I2C.AllAxes";
      }

      @Override
      public int getSize() {
        return DOUBLE_SIZE * 3;
      }

      @Override
      public String getSchema() {
        return "double x;double y;double z";
      }

      @Override
      public AllAxes unpack(ByteBuffer bb) {
        double x = bb.getDouble();
        double y = bb.getDouble();
        double z = bb.getDouble();
        return new AllAxes(x, y, z);
      }

      @Override
      public void pack(ByteBuffer bb, AllAxes value) {
        bb.putDouble(value.x);
        bb.putDouble(value.y);
        bb.putDouble(value.z);
      }

      @Override
      public boolean isImmutable() {
        return true;
      }
    }

    /** Struct implementation. */
    public static final AllAxesStruct struct = new AllAxesStruct();
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
   * @param range The range (+ or -) that the accelerometer will measure. Valid values are 2, 4, 8,
   *     or 16 Gs.
   */
  public ADXL345_I2C(I2C.Port port, int range) {
    this(port, range, DEFAULT_ADDRESS);
  }

  /**
   * Constructs the ADXL345 Accelerometer over I2C.
   *
   * @param port The I2C port the accelerometer is attached to
   * @param range The range (+ or -) that the accelerometer will measure. Valid values are 2, 4, 8,
   *     or 16 Gs.
   * @param deviceAddress I2C address of the accelerometer (0x1D or 0x53)
   */
  public ADXL345_I2C(I2C.Port port, int range, int deviceAddress) {
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
  }

  /**
   * Returns the I2C port.
   *
   * @return The I2C port.
   */
  public I2C.Port getPort() {
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
   *     measure. Valid values are 2, 4, 8, or 16 Gs.
   */
  public final void setRange(int range) {
    final byte value =
        switch (range) {
          case 2 -> 0;
          case 4 -> 1;
          case 8 -> 2;
          case 16 -> 3;
          default -> 0; // default to 2G if invalid value is passed in
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
    return getAcceleration(Axis.X);
  }

  /**
   * Returns the acceleration along the Y axis in g-forces.
   *
   * @return The acceleration along the Y axis in g-forces.
   */
  public double getY() {
    return getAcceleration(Axis.Y);
  }

  /**
   * Returns the acceleration along the Z axis in g-forces.
   *
   * @return The acceleration along the Z axis in g-forces.
   */
  public double getZ() {
    return getAcceleration(Axis.Z);
  }

  /**
   * Get the acceleration of one axis in Gs.
   *
   * @param axis The axis to read from.
   * @return Acceleration of the ADXL345 in Gs.
   */
  public double getAcceleration(Axis axis) {
    if (axis == Axis.X && m_simX != null) {
      return m_simX.get();
    }
    if (axis == Axis.Y && m_simY != null) {
      return m_simY.get();
    }
    if (axis == Axis.Z && m_simZ != null) {
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
    if (m_simX != null && m_simY != null && m_simZ != null) {
      return new AllAxes(m_simX.get(), m_simY.get(), m_simZ.get());
    }
    ByteBuffer rawData = ByteBuffer.allocate(6);
    m_i2c.read(DATA_REGISTER, 6, rawData);

    // Sensor is little endian... swap bytes
    rawData.order(ByteOrder.LITTLE_ENDIAN);
    return new AllAxes(
        rawData.getShort(0) * GS_PER_LSB,
        rawData.getShort(2) * GS_PER_LSB,
        rawData.getShort(4) * GS_PER_LSB);
  }

  @Override
  public void logTo(TelemetryTable table) {
    table.log("Value", getAccelerations());
  }

  @Override
  public String getTelemetryType() {
    return "3AxisAccelerometer";
  }
}
