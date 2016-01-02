/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteOrder;
import java.nio.ByteBuffer;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tInstances;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * ADXL362 SPI Accelerometer.
 *
 * This class allows access to an Analog Devices ADXL362 3-axis accelerometer.
 */
public class ADXL362 extends SensorBase implements Accelerometer, LiveWindowSendable {
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

  public static enum Axes {
    kX((byte) 0x00),
    kY((byte) 0x02),
    kZ((byte) 0x04);

    /**
     * The integer value representing this enumeration
     */
    public final byte value;

    private Axes(byte value) {
      this.value = value;
    }
  }

  public static class AllAxes {

    public double XAxis;
    public double YAxis;
    public double ZAxis;
  }

  private SPI m_spi;
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
   * @param port The SPI port that the accelerometer is connected to
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  public ADXL362(SPI.Port port, Range range) {
    m_spi = new SPI(port);
    m_spi.setClockRate(3000000);
    m_spi.setMSBFirst();
    m_spi.setSampleDataOnFalling();
    m_spi.setClockActiveLow();
    m_spi.setChipSelectActiveLow();

    // Validate the part ID
    ByteBuffer transferBuffer = ByteBuffer.allocateDirect(3);
    transferBuffer.put(0, kRegRead);
    transferBuffer.put(1, kPartIdRegister);
    m_spi.transaction(transferBuffer, transferBuffer, 3);
    if (transferBuffer.get(2) != (byte)0xF2) {
      m_spi.free();
      m_spi = null;
      DriverStation.reportError("could not find ADXL362 on SPI port " + port.getValue(), false);
      return;
    }

    setRange(range);

    // Turn on the measurements
    transferBuffer.put(0, kRegWrite);
    transferBuffer.put(1, kPowerCtlRegister);
    transferBuffer.put(2, (byte) (kPowerCtl_Measure | kPowerCtl_UltraLowNoise));
    m_spi.write(transferBuffer, 3);

    UsageReporting.report(tResourceType.kResourceType_ADXL362, port.getValue());
    LiveWindow.addSensor("ADXL362", port.getValue(), this);
  }

  public void free() {
    m_spi.free();
  }

  /** {inheritdoc} */
  @Override
  public void setRange(Range range) {
    byte value = 0;

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
    }

    // Specify the data format to read
    byte[] commands = new byte[] {kRegWrite, kFilterCtlRegister, (byte) (kFilterCtl_ODR_100Hz | value)};
    m_spi.write(commands, commands.length);
  }

  /** {@inheritDoc} */
  @Override
  public double getX() {
    return getAcceleration(Axes.kX);
  }

  /** {@inheritDoc} */
  @Override
  public double getY() {
    return getAcceleration(Axes.kY);
  }

  /** {@inheritDoc} */
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
    if (m_spi == null)
      return 0.0;
    ByteBuffer transferBuffer = ByteBuffer.allocateDirect(4);
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
   * @return An object containing the acceleration measured on each axis of the
   *         ADXL362 in Gs.
   */
  public ADXL362.AllAxes getAccelerations() {
    ADXL362.AllAxes data = new ADXL362.AllAxes();
    if (m_spi != null) {
      ByteBuffer dataBuffer = ByteBuffer.allocateDirect(8);
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

  public String getSmartDashboardType() {
    return "3AxisAccelerometer";
  }

  private ITable m_table;

  /** {@inheritDoc} */
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  /** {@inheritDoc} */
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("X", getX());
      m_table.putNumber("Y", getY());
      m_table.putNumber("Z", getZ());
    }
  }

  /** {@inheritDoc} */
  public ITable getTable() {
    return m_table;
  }

  public void startLiveWindowMode() {}

  public void stopLiveWindowMode() {}
}
