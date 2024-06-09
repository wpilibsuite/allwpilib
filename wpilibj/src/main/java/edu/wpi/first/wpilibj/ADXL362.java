// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
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

/**
 * ADXL362 SPI Accelerometer.
 *
 * <p>This class allows access to an Analog Devices ADXL362 3-axis accelerometer.
 */
public class ADXL362 implements NTSendable, AutoCloseable {
  private static final byte kRegWrite = 0x0A;
  private static final byte kRegRead = 0x0B;

  private static final byte kPartIdRegister = 0x02;
  private static final byte kDataRegister = 0x0E;
  private static final byte kFilterCtlRegister = 0x2C;
  private static final byte kPowerCtlRegister = 0x2D;

  private static final byte kFilterCtl_Range2G = 0x00;
  private static final byte kFilterCtl_Range4G = 0x40;
  private static final byte kFilterCtl_Range8G = (byte) 0x80;
  private static final byte kFilterCtl_ODR_100Hz = 0x03;

  private static final byte kPowerCtl_UltraLowNoise = 0x20;

  // @SuppressWarnings("PMD.UnusedPrivateField")
  // private static final byte kPowerCtl_AutoSleep = 0x04;

  private static final byte kPowerCtl_Measure = 0x02;

  /** Accelerometer range. */
  public enum Range {
    /** 2 Gs max. */
    k2G,
    /** 4 Gs max. */
    k4G,
    /** 8 Gs max. */
    k8G
  }

  /** Accelerometer axes. */
  public enum Axes {
    /** X axis. */
    kX((byte) 0x00),
    /** Y axis. */
    kY((byte) 0x02),
    /** Z axis. */
    kZ((byte) 0x04);

    /** Axis value. */
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

  private SPI m_spi;

  private SimDevice m_simDevice;
  private SimEnum m_simRange;
  private SimDouble m_simX;
  private SimDouble m_simY;
  private SimDouble m_simZ;

  private double m_gsPerLSB;

  /**
   * Constructor. Uses the onboard CS1.
   *
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  public ADXL362(Range range) {
    this(SPI.Port.kOnboardCS1, range);
  }

  /**
   * Constructor.
   *
   * @param port The SPI port that the accelerometer is connected to
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  @SuppressWarnings("this-escape")
  public ADXL362(SPI.Port port, Range range) {
    m_spi = new SPI(port);

    // simulation
    m_simDevice = SimDevice.create("Accel:ADXL362", port.value);
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

    m_spi.setClockRate(3000000);
    m_spi.setMode(SPI.Mode.kMode3);
    m_spi.setChipSelectActiveLow();

    ByteBuffer transferBuffer = ByteBuffer.allocate(3);
    if (m_simDevice == null) {
      // Validate the part ID
      transferBuffer.put(0, kRegRead);
      transferBuffer.put(1, kPartIdRegister);
      m_spi.transaction(transferBuffer, transferBuffer, 3);
      if (transferBuffer.get(2) != (byte) 0xF2) {
        m_spi.close();
        m_spi = null;
        DriverStation.reportError("could not find ADXL362 on SPI port " + port.value, false);
        return;
      }
    }

    setRange(range);

    // Turn on the measurements
    transferBuffer.put(0, kRegWrite);
    transferBuffer.put(1, kPowerCtlRegister);
    transferBuffer.put(2, (byte) (kPowerCtl_Measure | kPowerCtl_UltraLowNoise));
    m_spi.write(transferBuffer, 3);

    HAL.report(tResourceType.kResourceType_ADXL362, port.value + 1);
    SendableRegistry.addLW(this, "ADXL362", port.value);
  }

  /**
   * Returns the SPI port.
   *
   * @return The SPI port.
   */
  public int getPort() {
    return m_spi.getPort();
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    if (m_spi != null) {
      m_spi.close();
      m_spi = null;
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
    if (m_spi == null) {
      return;
    }

    final byte value =
        switch (range) {
          case k2G -> {
            m_gsPerLSB = 0.001;
            yield kFilterCtl_Range2G;
          }
          case k4G -> {
            m_gsPerLSB = 0.002;
            yield kFilterCtl_Range4G;
          }
          case k8G -> {
            m_gsPerLSB = 0.004;
            yield kFilterCtl_Range8G;
          }
        };

    // Specify the data format to read
    byte[] commands =
        new byte[] {kRegWrite, kFilterCtlRegister, (byte) (kFilterCtl_ODR_100Hz | value)};
    m_spi.write(commands, commands.length);

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
   * @return Acceleration of the ADXL362 in Gs.
   */
  public double getAcceleration(ADXL362.Axes axis) {
    if (axis == Axes.kX && m_simX != null) {
      return m_simX.get();
    }
    if (axis == Axes.kY && m_simY != null) {
      return m_simY.get();
    }
    if (axis == Axes.kZ && m_simZ != null) {
      return m_simZ.get();
    }
    if (m_spi == null) {
      return 0.0;
    }
    ByteBuffer transferBuffer = ByteBuffer.allocate(4);
    transferBuffer.put(0, kRegRead);
    transferBuffer.put(1, (byte) (kDataRegister + axis.value));
    m_spi.transaction(transferBuffer, transferBuffer, 4);
    // Sensor is little endian
    transferBuffer.order(ByteOrder.LITTLE_ENDIAN);

    return transferBuffer.getShort(2) * m_gsPerLSB;
  }

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the ADXL362 in Gs.
   */
  public ADXL362.AllAxes getAccelerations() {
    ADXL362.AllAxes data = new ADXL362.AllAxes();
    if (m_simX != null && m_simY != null && m_simZ != null) {
      data.XAxis = m_simX.get();
      data.YAxis = m_simY.get();
      data.ZAxis = m_simZ.get();
      return data;
    }
    if (m_spi != null) {
      ByteBuffer dataBuffer = ByteBuffer.allocate(8);
      // Select the data address.
      dataBuffer.put(0, kRegRead);
      dataBuffer.put(1, kDataRegister);
      m_spi.transaction(dataBuffer, dataBuffer, 8);
      // Sensor is little endian... swap bytes
      dataBuffer.order(ByteOrder.LITTLE_ENDIAN);

      data.XAxis = dataBuffer.getShort(2) * m_gsPerLSB;
      data.YAxis = dataBuffer.getShort(4) * m_gsPerLSB;
      data.ZAxis = dataBuffer.getShort(6) * m_gsPerLSB;
    }
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
