// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Use a rate gyro to return the robots heading relative to a starting position. The Gyro class
 * tracks the robots heading based on the starting position. As the robot rotates the new heading is
 * computed by integrating the rate of rotation returned by the sensor. When the class is
 * instantiated, it does a short calibration routine where it samples the gyro while at rest to
 * determine the default offset. This is subtracted from each sample to determine the heading.
 *
 * <p>This class is for the digital ADXRS450 gyro sensor that connects via SPI. Only one instance of
 * an ADXRS Gyro is supported.
 */
@SuppressWarnings("TypeName")
public class ADXRS450_Gyro implements Sendable, AutoCloseable {
  private static final double kSamplePeriod = 0.0005;
  private static final double kCalibrationSampleTime = 5.0;
  private static final double kDegreePerSecondPerLSB = 0.0125;

  // private static final int kRateRegister = 0x00;
  // private static final int kTemRegister = 0x02;
  // private static final int kLoCSTRegister = 0x04;
  // private static final int kHiCSTRegister = 0x06;
  // private static final int kQuadRegister = 0x08;
  // private static final int kFaultRegister = 0x0A;
  private static final int kPIDRegister = 0x0C;
  // private static final int kSNHighRegister = 0x0E;
  // private static final int kSNLowRegister = 0x10;

  private SPI m_spi;

  private SimDevice m_simDevice;
  private SimBoolean m_simConnected;
  private SimDouble m_simAngle;
  private SimDouble m_simRate;

  /** Constructor. Uses the onboard CS0. */
  public ADXRS450_Gyro() {
    this(SPI.Port.kOnboardCS0);
  }

  /**
   * Constructor.
   *
   * @param port The SPI port that the gyro is connected to
   */
  @SuppressWarnings("this-escape")
  public ADXRS450_Gyro(SPI.Port port) {
    m_spi = new SPI(port);

    // simulation
    m_simDevice = SimDevice.create("Gyro:ADXRS450", port.value);
    if (m_simDevice != null) {
      m_simConnected = m_simDevice.createBoolean("connected", SimDevice.Direction.kInput, true);
      m_simAngle = m_simDevice.createDouble("angle_x", SimDevice.Direction.kInput, 0.0);
      m_simRate = m_simDevice.createDouble("rate_x", SimDevice.Direction.kInput, 0.0);
    }

    m_spi.setClockRate(3000000);
    m_spi.setMode(SPI.Mode.kMode0);
    m_spi.setChipSelectActiveLow();

    if (m_simDevice == null) {
      // Validate the part ID
      if ((readRegister(kPIDRegister) & 0xff00) != 0x5200) {
        m_spi.close();
        m_spi = null;
        DriverStation.reportError("could not find ADXRS450 gyro on SPI port " + port.value, false);
        return;
      }

      m_spi.initAccumulator(
          kSamplePeriod, 0x20000000, 4, 0x0c00000e, 0x04000000, 10, 16, true, true);

      calibrate();
    }

    HAL.report(tResourceType.kResourceType_ADXRS450, port.value + 1);
    SendableRegistry.addLW(this, "ADXRS450_Gyro", port.value);
  }

  /**
   * Determine if the gyro is connected.
   *
   * @return true if it is connected, false otherwise.
   */
  public boolean isConnected() {
    if (m_simConnected != null) {
      return m_simConnected.get();
    }
    return m_spi != null;
  }

  /**
   * Calibrate the gyro by running for a number of samples and computing the center value. Then use
   * the center value as the Accumulator center value for subsequent measurements.
   *
   * <p>It's important to make sure that the robot is not moving while the centering calculations
   * are in progress, this is typically done when the robot is first turned on while it's sitting at
   * rest before the competition starts.
   */
  public final void calibrate() {
    if (m_spi == null) {
      return;
    }

    Timer.delay(0.1);

    m_spi.setAccumulatorIntegratedCenter(0);
    m_spi.resetAccumulator();

    Timer.delay(kCalibrationSampleTime);

    m_spi.setAccumulatorIntegratedCenter(m_spi.getAccumulatorIntegratedAverage());
    m_spi.resetAccumulator();
  }

  /**
   * Get the SPI port number.
   *
   * @return The SPI port number.
   */
  public int getPort() {
    return m_spi.getPort();
  }

  private boolean calcParity(int value) {
    boolean parity = false;
    while (value != 0) {
      parity = !parity;
      value = value & (value - 1);
    }
    return parity;
  }

  private int readRegister(int reg) {
    int cmdhi = 0x8000 | (reg << 1);
    boolean parity = calcParity(cmdhi);

    ByteBuffer buf = ByteBuffer.allocate(4);
    buf.order(ByteOrder.BIG_ENDIAN);
    buf.put(0, (byte) (cmdhi >> 8));
    buf.put(1, (byte) (cmdhi & 0xff));
    buf.put(2, (byte) 0);
    buf.put(3, (byte) (parity ? 0 : 1));

    m_spi.write(buf, 4);
    m_spi.read(false, buf, 4);

    if ((buf.get(0) & 0xe0) == 0) {
      return 0; // error, return 0
    }
    return (buf.getInt(0) >> 5) & 0xffff;
  }

  /**
   * Reset the gyro.
   *
   * <p>Resets the gyro to a heading of zero. This can be used if there is significant drift in the
   * gyro, and it needs to be recalibrated after it has been running.
   */
  public void reset() {
    if (m_simAngle != null) {
      m_simAngle.reset();
    }
    if (m_spi != null) {
      m_spi.resetAccumulator();
    }
  }

  /** Delete (free) the spi port used for the gyro and stop accumulating. */
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
   * Return the heading of the robot in degrees.
   *
   * <p>The angle is continuous, that is it will continue from 360 to 361 degrees. This allows
   * algorithms that wouldn't want to see a discontinuity in the gyro output as it sweeps past from
   * 360 to 0 on the second time around.
   *
   * <p>The angle is expected to increase as the gyro turns clockwise when looked at from the top.
   * It needs to follow the NED axis convention.
   *
   * <p>This heading is based on integration of the returned rate from the gyro.
   *
   * @return the current heading of the robot in degrees.
   */
  public double getAngle() {
    if (m_simAngle != null) {
      return m_simAngle.get();
    }
    if (m_spi == null) {
      return 0.0;
    }
    return m_spi.getAccumulatorIntegratedValue() * kDegreePerSecondPerLSB;
  }

  /**
   * Return the rate of rotation of the gyro.
   *
   * <p>The rate is based on the most recent reading of the gyro analog value
   *
   * <p>The rate is expected to be positive as the gyro turns clockwise when looked at from the top.
   * It needs to follow the NED axis convention.
   *
   * @return the current rate in degrees per second
   */
  public double getRate() {
    if (m_simRate != null) {
      return m_simRate.get();
    }
    if (m_spi == null) {
      return 0.0;
    }
    return m_spi.getAccumulatorLastValue() * kDegreePerSecondPerLSB;
  }

  /**
   * Return the heading of the robot as a {@link edu.wpi.first.math.geometry.Rotation2d}.
   *
   * <p>The angle is continuous, that is it will continue from 360 to 361 degrees. This allows
   * algorithms that wouldn't want to see a discontinuity in the gyro output as it sweeps past from
   * 360 to 0 on the second time around.
   *
   * <p>The angle is expected to increase as the gyro turns counterclockwise when looked at from the
   * top. It needs to follow the NWU axis convention.
   *
   * <p>This heading is based on integration of the returned rate from the gyro.
   *
   * @return the current heading of the robot as a {@link edu.wpi.first.math.geometry.Rotation2d}.
   */
  public Rotation2d getRotation2d() {
    return Rotation2d.fromDegrees(-getAngle());
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Gyro");
    builder.addDoubleProperty("Value", this::getAngle, null);
  }
}
