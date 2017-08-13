/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tInstances;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * ADXL345 I2C Accelerometer.
 */
@SuppressWarnings({"TypeName", "PMD.UnusedPrivateField"})
public class ADXL345_I2C extends SensorBase implements Accelerometer, LiveWindowSendable {

  private static final byte kAddress = 0x1D;
  private static final byte kPowerCtlRegister = 0x2D;
  private static final byte kDataFormatRegister = 0x31;
  private static final byte kDataRegister = 0x32;
  private static final double kGsPerLSB = 0.00390625;
  private static final byte kPowerCtl_Link = 0x20;
  private static final byte kPowerCtl_AutoSleep = 0x10;
  private static final byte kPowerCtl_Measure = 0x08;
  private static final byte kPowerCtl_Sleep = 0x04;

  private static final byte kDataFormat_SelfTest = (byte) 0x80;
  private static final byte kDataFormat_SPI = 0x40;
  private static final byte kDataFormat_IntInvert = 0x20;
  private static final byte kDataFormat_FullRes = 0x08;
  private static final byte kDataFormat_Justify = 0x04;

  public enum Axes {
    kX((byte) 0x00),
    kY((byte) 0x02),
    kZ((byte) 0x04);

    /**
     * The integer value representing this enumeration.
     */
    @SuppressWarnings("MemberName")
    public final byte value;

    private Axes(byte value) {
      this.value = value;
    }
  }

  @SuppressWarnings("MemberName")
  public static class AllAxes {

    public double XAxis;
    public double YAxis;
    public double ZAxis;
  }

  protected I2C m_i2c;

  /**
   * Constructs the ADXL345 Accelerometer with I2C address 0x1D.
   *
   * @param port  The I2C port the accelerometer is attached to
   * @param range The range (+ or -) that the accelerometer will measure.
   */
  public ADXL345_I2C(I2C.Port port, Range range) {
    this(port, range, kAddress);
  }

  /**
   * Constructs the ADXL345 Accelerometer over I2C.
   *
   * @param port          The I2C port the accelerometer is attached to
   * @param range         The range (+ or -) that the accelerometer will measure.
   * @param deviceAddress I2C address of the accelerometer (0x1D or 0x53)
   */
  public ADXL345_I2C(I2C.Port port, Range range, int deviceAddress) {
    m_i2c = new I2C(port, deviceAddress);

    // Turn on the measurements
    m_i2c.write(kPowerCtlRegister, kPowerCtl_Measure);

    setRange(range);

    HAL.report(tResourceType.kResourceType_ADXL345, tInstances.kADXL345_I2C);
    LiveWindow.addSensor("ADXL345_I2C", port.value, this);
  }


  @Override
  public void setRange(Range range) {
    final byte value;

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
      default:
        throw new IllegalArgumentException(range + " unsupported range type");
    }

    // Specify the data format to read
    m_i2c.write(kDataFormatRegister, kDataFormat_FullRes | value);
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
   * @return Acceleration of the ADXL345 in Gs.
   */
  public double getAcceleration(Axes axis) {
    ByteBuffer rawAccel = ByteBuffer.allocateDirect(2);
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
    AllAxes data = new AllAxes();
    ByteBuffer rawData = ByteBuffer.allocateDirect(6);
    m_i2c.read(kDataRegister, 6, rawData);

    // Sensor is little endian... swap bytes
    rawData.order(ByteOrder.LITTLE_ENDIAN);
    data.XAxis = rawData.getShort(0) * kGsPerLSB;
    data.YAxis = rawData.getShort(2) * kGsPerLSB;
    data.ZAxis = rawData.getShort(4) * kGsPerLSB;
    return data;
  }

  @Override
  public String getSmartDashboardType() {
    return "3AxisAccelerometer";
  }

  private ITable m_table;

  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("X", getX());
      m_table.putNumber("Y", getY());
      m_table.putNumber("Z", getZ());
    }
  }

  @Override
  public ITable getTable() {
    return m_table;
  }

  @Override
  public void startLiveWindowMode() {
  }

  @Override
  public void stopLiveWindowMode() {
  }
}
