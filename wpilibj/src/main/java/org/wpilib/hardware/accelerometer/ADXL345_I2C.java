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
  public static final byte kAddress = 0x1D;

  private static final byte kPowerCtlRegister = 0x2D;
  private static final byte kDataFormatRegister = 0x31;
  private static final byte kDataRegister = 0x32;
  private static final double kGsPerLSB = 0.00390625;
  // private static final byte kPowerCtl_Link = 0x20;
  // private static final byte kPowerCtl_AutoSleep = 0x10;
  private static final byte kPowerCtl_Measure = 0x08;
  // private static final byte kPowerCtl_Sleep = 0x04;

  // private static final byte kDataFormat_SelfTest = (byte) 0x80;
  // private static final byte kDataFormat_SPI = 0x40;
  // private static final byte kDataFormat_IntInvert = 0x20;
  private static final byte kDataFormat_FullRes = 0x08;

  // private static final byte kDataFormat_Justify = 0x04;

  /** Accelerometer range. */
  public enum Range {
    /** 2 Gs max. */
    k2G,
    /** 4 Gs max. */
    k4G,
    /** 8 Gs max. */
    k8G,
    /** 16 Gs max. */
    k16G
  }

  /** Accelerometer axes. */
  public enum Axes {
    /** X axis. */
    kX((byte) 0x00),
    /** Y axis. */
    kY((byte) 0x02),
    /** Z axis. */
    kZ((byte) 0x04);

    /** The integer value representing this enumeration. */
    public final byte value;

    Axes(byte value) {
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
        return kSizeDouble * 3;
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
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  public ADXL345_I2C(I2C.Port port, Range range) {
    this(port, range, kAddress);
  }

  /**
   * Constructs the ADXL345 Accelerometer over I2C.
   *
   * @param port The I2C port the accelerometer is attached to
   * @param range The range (+ or -) that the accelerometer will measure.
   * @param deviceAddress I2C address of the accelerometer (0x1D or 0x53)
   */
  public ADXL345_I2C(I2C.Port port, Range range, int deviceAddress) {
    m_i2c = new I2C(port, deviceAddress);

    // simulation
    m_simDevice = SimDevice.create("Accel:ADXL345_I2C", port.value, deviceAddress);
    if (m_simDevice != null) {
      m_simRange =
          m_simDevice.createEnumDouble(
              "range",
              SimDevice.Direction.kOutput,
              new String[] {"2G", "4G", "8G", "16G"},
              new double[] {2.0, 4.0, 8.0, 16.0},
              0);
      m_simX = m_simDevice.createDouble("x", SimDevice.Direction.kInput, 0.0);
      m_simY = m_simDevice.createDouble("y", SimDevice.Direction.kInput, 0.0);
      m_simZ = m_simDevice.createDouble("z", SimDevice.Direction.kInput, 0.0);
    }

    // Turn on the measurements
    m_i2c.write(kPowerCtlRegister, kPowerCtl_Measure);

    setRange(range);

    HAL.reportUsage("I2C[" + port.value + "][" + deviceAddress + "]", "ADXL345");
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
          case k2G -> 0;
          case k4G -> 1;
          case k8G -> 2;
          case k16G -> 3;
        };

    // Specify the data format to read
    m_i2c.write(kDataFormatRegister, kDataFormat_FullRes | value);

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
    return getAcceleration(Axes.kX);
  }

  /**
   * Returns the acceleration along the Y axis in g-forces.
   *
   * @return The acceleration along the Y axis in g-forces.
   */
  public double getY() {
    return getAcceleration(Axes.kY);
  }

  /**
   * Returns the acceleration along the Z axis in g-forces.
   *
   * @return The acceleration along the Z axis in g-forces.
   */
  public double getZ() {
    return getAcceleration(Axes.kZ);
  }

  /**
   * Get the acceleration of one axis in Gs.
   *
   * @param axis The axis to read from.
   * @return Acceleration of the ADXL345 in Gs.
   */
  public double getAcceleration(Axes axis) {
    if (axis == Axes.kX && m_simX != null) {
      return m_simX.get();
    }
    if (axis == Axes.kY && m_simY != null) {
      return m_simY.get();
    }
    if (axis == Axes.kZ && m_simZ != null) {
      return m_simZ.get();
    }
    ByteBuffer rawAccel = ByteBuffer.allocate(2);
    m_i2c.read(kDataRegister + axis.value, 2, rawAccel);

    // Sensor is little endian... swap bytes
    rawAccel.order(ByteOrder.LITTLE_ENDIAN);
    return rawAccel.getShort(0) * kGsPerLSB;
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
    m_i2c.read(kDataRegister, 6, rawData);

    // Sensor is little endian... swap bytes
    rawData.order(ByteOrder.LITTLE_ENDIAN);
    return new AllAxes(
        rawData.getShort(0) * kGsPerLSB,
        rawData.getShort(2) * kGsPerLSB,
        rawData.getShort(4) * kGsPerLSB);
  }

  @Override
  public void updateTelemetry(TelemetryTable table) {
    table.log("Value", getAccelerations());
  }

  @Override
  public String getTelemetryType() {
    return "3AxisAccelerometer";
  }
}
