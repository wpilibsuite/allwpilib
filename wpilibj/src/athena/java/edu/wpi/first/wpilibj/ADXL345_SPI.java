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
 *
 * @author dtjones
 * @author mwills
 */
public class ADXL345_SPI extends SensorBase implements Accelerometer, LiveWindowSendable {
  private static final int kPowerCtlRegister = 0x2D;
  private static final int kDataFormatRegister = 0x31;
  private static final int kDataRegister = 0x32;
  private static final double kGsPerLSB = 0.00390625;

  private static final int kAddress_Read = 0x80;
  private static final int kAddress_MultiByte = 0x40;

  private static final int kPowerCtl_Link = 0x20;
  private static final int kPowerCtl_AutoSleep = 0x10;
  private static final int kPowerCtl_Measure = 0x08;
  private static final int kPowerCtl_Sleep = 0x04;

  private static final int kDataFormat_SelfTest = 0x80;
  private static final int kDataFormat_SPI = 0x40;
  private static final int kDataFormat_IntInvert = 0x20;
  private static final int kDataFormat_FullRes = 0x08;
  private static final int kDataFormat_Justify = 0x04;

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

  /**
   * Constructor.
   *
   * @param port The SPI port that the accelerometer is connected to
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  public ADXL345_SPI(SPI.Port port, Range range) {
    m_spi = new SPI(port);
    init(range);
    LiveWindow.addSensor("ADXL345_SPI", port.getValue(), this);
  }

  public void free() {
    m_spi.free();
  }

  /**
   * Set SPI bus parameters, bring device out of sleep and set format
   *
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  private void init(Range range) {
    m_spi.setClockRate(500000);
    m_spi.setMSBFirst();
    m_spi.setSampleDataOnFalling();
    m_spi.setClockActiveLow();
    m_spi.setChipSelectActiveHigh();

    // Turn on the measurements
    byte[] commands = new byte[2];
    commands[0] = kPowerCtlRegister;
    commands[1] = kPowerCtl_Measure;
    m_spi.write(commands, 2);

    setRange(range);

    UsageReporting.report(tResourceType.kResourceType_ADXL345, tInstances.kADXL345_SPI);
  }

  /** {inheritdoc} */
  @Override
  public void setRange(Range range) {
    byte value = 0;

    switch (range) {
      case k2G:
        value = 0;
        break;
      case k4G:
        value = 1;
        break;
      case k8G:
        value = 2;
        break;
      case k16G:
        value = 3;
        break;
    }

    // Specify the data format to read
    byte[] commands = new byte[] {kDataFormatRegister, (byte) (kDataFormat_FullRes | value)};
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
   * @return Acceleration of the ADXL345 in Gs.
   */
  public double getAcceleration(ADXL345_SPI.Axes axis) {
    ByteBuffer transferBuffer = ByteBuffer.allocateDirect(3);
    transferBuffer.put(0, (byte) ((kAddress_Read | kAddress_MultiByte | kDataRegister) + axis.value));
    m_spi.transaction(transferBuffer, transferBuffer, 3);
    // Sensor is little endian
    transferBuffer.order(ByteOrder.LITTLE_ENDIAN);

    return transferBuffer.getShort(1) * kGsPerLSB;
  }

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the
   *         ADXL345 in Gs.
   */
  public ADXL345_SPI.AllAxes getAccelerations() {
    ADXL345_SPI.AllAxes data = new ADXL345_SPI.AllAxes();
    if (m_spi != null) {
      ByteBuffer dataBuffer = ByteBuffer.allocateDirect(7);
      // Select the data address.
      dataBuffer.put(0, (byte) (kAddress_Read | kAddress_MultiByte | kDataRegister));
      m_spi.transaction(dataBuffer, dataBuffer, 7);
      // Sensor is little endian... swap bytes
      dataBuffer.order(ByteOrder.LITTLE_ENDIAN);

      data.XAxis = dataBuffer.getShort(1) * kGsPerLSB;
      data.YAxis = dataBuffer.getShort(3) * kGsPerLSB;
      data.ZAxis = dataBuffer.getShort(5) * kGsPerLSB;
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
