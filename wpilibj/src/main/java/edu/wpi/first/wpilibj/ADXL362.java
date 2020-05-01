/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.hal.SimEnum;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * ADXL362 SPI Accelerometer.
 *
 * <p>This class allows access to an Analog Devices ADXL362 3-axis accelerometer.
 */
@SuppressWarnings("PMD.UnusedPrivateField")
public class ADXL362 implements Accelerometer, Sendable, AutoCloseable {
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
  private static final byte kPowerCtl_AutoSleep = 0x04;
  private static final byte kPowerCtl_Measure = 0x02;

  public enum Axes {
    kX((byte) 0x00),
    kY((byte) 0x02),
    kZ((byte) 0x04);

    @SuppressWarnings("MemberName")
    public final byte value;

    Axes(byte value) {
      this.value = value;
    }
  }

  @SuppressWarnings("MemberName")
  public static class AllAxes {
    public double XAxis;
    public double YAxis;
    public double ZAxis;
  }

  private SPI m_spi;

  private SimDevice m_simDevice;
  private SimEnum m_simRange;
  private SimDouble m_simX;
  private SimDouble m_simY;
  private SimDouble m_simZ;

  private double m_gsPerLSB;

  /**
   * Constructor.  Uses the onboard CS1.
   *
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  public ADXL362(Range range) {
    this(SPI.Port.kOnboardCS1, range);
  }

  /**
   * Constructor.
   *
   * @param port  The SPI port that the accelerometer is connected to
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  public ADXL362(SPI.Port port, Range range) {
    m_spi = new SPI(port);

    // simulation
    m_simDevice = SimDevice.create("ADXL362", port.value);
    if (m_simDevice != null) {
      m_simRange = m_simDevice.createEnum("Range", true, new String[] {"2G", "4G", "8G", "16G"}, 0);
      m_simX = m_simDevice.createDouble("X Accel", false, 0.0);
      m_simY = m_simDevice.createDouble("Y Accel", false, 0.0);
      m_simZ = m_simDevice.createDouble("Z Accel", false, 0.0);
    }

    m_spi.setClockRate(3000000);
    m_spi.setMSBFirst();
    m_spi.setSampleDataOnTrailingEdge();
    m_spi.setClockActiveLow();
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

  @Override
  public void setRange(Range range) {
    if (m_spi == null) {
      return;
    }

    final byte value;
    switch (range) {
      case k2G:
        value = kFilterCtl_Range2G;
        m_gsPerLSB = 0.001;
        break;
      case k4G:
        value = kFilterCtl_Range4G;
        m_gsPerLSB = 0.002;
        break;
      case k8G:
      case k16G:  // 16G not supported; treat as 8G
        value = kFilterCtl_Range8G;
        m_gsPerLSB = 0.004;
        break;
      default:
        throw new IllegalArgumentException(range + " unsupported");

    }

    // Specify the data format to read
    byte[] commands = new byte[]{kRegWrite, kFilterCtlRegister, (byte) (kFilterCtl_ODR_100Hz
        | value)};
    m_spi.write(commands, commands.length);

    if (m_simRange != null) {
      m_simRange.set(value);
    }
  }


  @Override
  public double getX() {
    return getAcceleration(Axes.kX);
  }

  @Override
  public double getY() {
    return getAcceleration(Axes.kY);
  }

  @Override
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
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("3AxisAccelerometer");
    NetworkTableEntry entryX = builder.getEntry("X");
    NetworkTableEntry entryY = builder.getEntry("Y");
    NetworkTableEntry entryZ = builder.getEntry("Z");
    builder.setUpdateTable(() -> {
      AllAxes data = getAccelerations();
      entryX.setDouble(data.XAxis);
      entryY.setDouble(data.YAxis);
      entryZ.setDouble(data.ZAxis);
    });
  }
}
