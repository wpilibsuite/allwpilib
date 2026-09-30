// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//
// This file is based on the FTC SDK drivers for the AMS TMD3782 family of color
// sensors, which were made available under the BSD 3-Clause License.
// Copyright (c) 2016-2017 Robert Atkinson and Steve Geffner.

package org.wpilib.drivers.range;

import static org.wpilib.units.Units.Meters;
import static org.wpilib.units.Units.Milliseconds;

import org.wpilib.hardware.bus.I2C;
import org.wpilib.units.measure.Distance;
import org.wpilib.units.measure.Time;
import org.wpilib.util.Color;
import org.wpilib.util.ErrorMessages;
import org.wpilib.util.UsageReporting;

/**
 * Driver for the REV Robotics Color Sensor V2.
 *
 * <p>The sensor is an AMS TMD37821, which combines an RGBC color sensor with an infrared proximity
 * sensor. The proximity channel is exposed both as a calibrated distance and as a raw reflected
 * light level.
 *
 * <p>Call {@link #update()} once per robot loop. All accessors return the values cached by the most
 * recent update; they do not access the device.
 *
 * <p>Aborted I2C transactions, an unexpected device ID, and data the sensor reports as not yet
 * valid are reported through {@link DeviceStatus} instead of throwing an exception, and the
 * previously cached measurements are preserved. Unless otherwise noted, methods that access the
 * device throw {@link IllegalStateException} after this driver is closed.
 */
public class RevColorSensorV2 implements AutoCloseable {
  /** Default 7-bit I2C address of the Color Sensor V2. */
  public static final int DEFAULT_ADDRESS = 0x39;

  /** Every register access is prefixed with the command bit. */
  private static final int COMMAND_BIT = 0x80;

  /**
   * Command type selecting the auto-increment protocol. Without it the sensor uses the
   * repeated-byte protocol, which returns the addressed register once per byte instead of advancing
   * through consecutive registers, so a multi-byte read would return the same register repeatedly.
   */
  private static final int COMMAND_TYPE_AUTO_INCREMENT = 0x01 << 5;

  /** Register address prefix used for every read and write. */
  private static final int COMMAND_AUTO_INCREMENT = COMMAND_BIT | COMMAND_TYPE_AUTO_INCREMENT;

  private static final byte TMD37821_DEVICE_ID = 0x60;
  private static final byte TMD37823_DEVICE_ID = 0x69;

  private static final int ENABLE_POWER_ON = 0x01;
  private static final int ENABLE_COLOR = 0x02;
  private static final int ENABLE_PROXIMITY = 0x04;
  private static final int ENABLE_WAIT = 0x08;
  private static final int ENABLE_WRITABLE_MASK =
      ENABLE_POWER_ON | ENABLE_COLOR | ENABLE_PROXIMITY | ENABLE_WAIT;

  private static final int STATUS_COLOR_VALID = 0x01;
  private static final int STATUS_PROXIMITY_VALID = 0x02;

  /**
   * CONTROL proximity diode select field (bits 5:4), set to the value that measures proximity with
   * the infrared diode. The other values of this field are reserved.
   */
  private static final int CONTROL_PDIODE_IR = 0x02 << 4;

  /** STATUS through the high byte of the proximity data, read as one block. */
  private static final int BULK_READ_LENGTH = 11;

  private static final int CLEAR_OFFSET = 1;
  private static final int RED_OFFSET = 3;
  private static final int GREEN_OFFSET = 5;
  private static final int BLUE_OFFSET = 7;
  private static final int PROXIMITY_OFFSET = 9;

  private static final double INTEGRATION_CYCLE_MILLISECONDS = 2.4;
  private static final int MAX_INTEGRATION_CYCLES = 256;
  private static final int COUNTS_PER_INTEGRATION_CYCLE = 1024;
  private static final int MAX_RAW_COLOR_VALUE = 65535;
  private static final int PROXIMITY_SATURATION = 1023;

  /** The datasheet specifies a 2.4 ms warm-up delay after power on. */
  private static final int POWER_ON_DELAY_MILLISECONDS = 3;

  private static final int ENABLE_SETTLE_DELAY_MILLISECONDS = 5;

  private static final double DEFAULT_INTEGRATION_TIME_MILLISECONDS = 24.0;
  private static final int DEFAULT_PROXIMITY_PULSE_COUNT = 8;

  private static final double DEFAULT_A_PARAM = 186.347;
  private static final double DEFAULT_B_PARAM = 30403.5;
  private static final double DEFAULT_C_PARAM = 0.576649;

  /** Register map for the TMD37821. */
  public enum Register {
    /** Enables states and interrupts. */
    ENABLE(0x00),
    /** RGBC integration time. */
    ATIME(0x01),
    /** Wait time. */
    WTIME(0x03),
    /** Clear channel interrupt low threshold. */
    AILT(0x04),
    /** Clear channel interrupt high threshold. */
    AIHT(0x06),
    /** Interrupt persistence filter. */
    PERS(0x0C),
    /** Configuration. */
    CONFIGURATION(0x0D),
    /** Proximity LED pulse count. */
    PPULSE(0x0E),
    /** Gain and proximity LED drive control. */
    CONTROL(0x0F),
    /** Device identifier. */
    DEVICE_ID(0x12),
    /** Device status. */
    STATUS(0x13),
    /** Clear channel data. */
    CLEAR(0x14),
    /** Red channel data. */
    RED(0x16),
    /** Green channel data. */
    GREEN(0x18),
    /** Blue channel data. */
    BLUE(0x1A),
    /** Proximity data. */
    PROXIMITY(0x1C);

    private final int m_address;

    Register(int address) {
      m_address = address;
    }

    /**
     * Returns the address of this register.
     *
     * @return register address
     */
    public int getAddress() {
      return m_address;
    }
  }

  /** Analog gain applied to the color channels by the sensor. */
  public enum Gain {
    /** 1x gain. */
    GAIN_1(0x00),
    /** 4x gain. */
    GAIN_4(0x01),
    /** 16x gain. */
    GAIN_16(0x02),
    /** 60x gain. */
    GAIN_60(0x03);

    private final int m_value;

    Gain(int value) {
      m_value = value;
    }
  }

  /** Nominal drive current of the proximity LED. */
  public enum LedDrive {
    /** 100% drive current. */
    PERCENT_100(0x00 << 6),
    /** 50% drive current. */
    PERCENT_50(0x01 << 6),
    /** 25% drive current. */
    PERCENT_25(0x02 << 6),
    /** 12.5% drive current. */
    PERCENT_12_5(0x03 << 6);

    private final int m_value;

    LedDrive(int value) {
      m_value = value;
    }
  }

  /** Sensor status, including device-reported faults and local read failures. */
  public enum DeviceStatus {
    /** The sensor has not been configured successfully yet. */
    NOT_INITIALIZED,
    /** The sensor is operating normally. */
    READY,
    /** The device ID register did not report a TMD3782 family part. */
    FAULT_UNEXPECTED_DEVICE_ID,
    /** An I2C transaction was aborted, or the sensor reported that its data was not valid. */
    FAULT_BAD_READ
  }

  /** Reason the driver rejected data or an I2C transaction. */
  public enum FailureReason {
    /** The I2C controller aborted a read transaction. */
    I2C_READ_ABORTED,
    /** The I2C controller aborted a write transaction. */
    I2C_WRITE_ABORTED,
    /** The device ID register did not report a TMD3782 family part. */
    UNEXPECTED_DEVICE_ID,
    /** The sensor had not completed an RGBC cycle since the color channels were enabled. */
    COLOR_DATA_NOT_VALID,
    /** The sensor had not completed a proximity cycle since the proximity channel was enabled. */
    PROXIMITY_DATA_NOT_VALID
  }

  private I2C m_i2c;

  private Gain m_gain = Gain.GAIN_4;
  private LedDrive m_ledDrive = LedDrive.PERCENT_50;
  private int m_integrationTimeRegister =
      integrationTimeRegister(DEFAULT_INTEGRATION_TIME_MILLISECONDS);
  private int m_proximityPulseCount = DEFAULT_PROXIMITY_PULSE_COUNT;
  private double m_softwareGain = 1.0;

  private double m_aParam = DEFAULT_A_PARAM;
  private double m_bParam = DEFAULT_B_PARAM;
  private double m_cParam = DEFAULT_C_PARAM;

  private boolean m_initialized;
  private byte m_deviceId;
  private DeviceStatus m_deviceStatus = DeviceStatus.NOT_INITIALIZED;
  private FailureReason m_lastFailureReason;
  private long m_failureCount;

  private int m_rawClear;
  private int m_rawRed;
  private int m_rawGreen;
  private int m_rawBlue;
  private int m_rawProximity;

  /**
   * Constructs a Color Sensor V2 on its default I2C address and configures it.
   *
   * @param port I2C port to which the sensor is connected
   * @throws NullPointerException if {@code port} is null
   */
  public RevColorSensorV2(I2C.Port port) {
    this(port, DEFAULT_ADDRESS);
  }

  /**
   * Constructs a Color Sensor V2 and configures it.
   *
   * <p>Configuration failures are reported through {@link #getDeviceStatus()}; {@link #update()}
   * retries configuration until it succeeds.
   *
   * @param port I2C port to which the sensor is connected
   * @param deviceAddress 7-bit I2C address
   * @throws NullPointerException if {@code port} is null
   * @throws IllegalArgumentException if {@code deviceAddress} is outside the 7-bit address range
   */
  public RevColorSensorV2(I2C.Port port, int deviceAddress) {
    ErrorMessages.requireNonNullParam(port, "port", "RevColorSensorV2");
    if (deviceAddress < 0 || deviceAddress > 0x7f) {
      throw new IllegalArgumentException("deviceAddress must be a 7-bit I2C address");
    }

    m_i2c = new I2C(port, deviceAddress);
    UsageReporting.reportUsage("I2C[" + port.value + "]", deviceAddress, "RevColorSensorV2");
    initialize();
  }

  RevColorSensorV2(I2C i2c) {
    m_i2c = ErrorMessages.requireNonNullParam(i2c, "i2c", "RevColorSensorV2");
    initialize();
  }

  /**
   * Returns the I2C port.
   *
   * @return I2C port
   * @throws IllegalStateException if this driver has been closed
   */
  public I2C.Port getPort() {
    return requireOpen().getPort();
  }

  /**
   * Returns the I2C address.
   *
   * @return 7-bit I2C address
   * @throws IllegalStateException if this driver has been closed
   */
  public int getDeviceAddress() {
    return requireOpen().getDeviceAddress();
  }

  /** Closes the I2C connection. Calling this method more than once has no effect. */
  @Override
  public void close() {
    if (m_i2c != null) {
      m_i2c.close();
      m_i2c = null;
    }
  }

  /**
   * Reads the color and proximity channels and updates the cached measurements. Call this once per
   * robot loop.
   *
   * <p>If the sensor has not been configured successfully, this retries configuration first, which
   * blocks for approximately 8 ms.
   *
   * <p>An aborted I2C transaction or data the sensor reports as not yet valid does not throw; it
   * sets the device status to {@link DeviceStatus#FAULT_BAD_READ} and preserves the affected
   * measurements.
   *
   * @throws IllegalStateException if this driver has been closed
   */
  public void update() {
    requireOpen();
    if (!m_initialized && !initialize()) {
      return;
    }

    m_deviceStatus = DeviceStatus.READY;
    byte[] data = readRegister(Register.STATUS, BULK_READ_LENGTH);
    if (data.length == 0) {
      return;
    }

    int status = data[0] & 0xFF;
    if ((status & STATUS_COLOR_VALID) == 0) {
      recordFailure(FailureReason.COLOR_DATA_NOT_VALID);
    } else {
      m_rawClear = decodeUnsignedShort(data, CLEAR_OFFSET);
      m_rawRed = decodeUnsignedShort(data, RED_OFFSET);
      m_rawGreen = decodeUnsignedShort(data, GREEN_OFFSET);
      m_rawBlue = decodeUnsignedShort(data, BLUE_OFFSET);
    }

    if ((status & STATUS_PROXIMITY_VALID) == 0) {
      recordFailure(FailureReason.PROXIMITY_DATA_NOT_VALID);
    } else {
      m_rawProximity = decodeUnsignedShort(data, PROXIMITY_OFFSET);
    }
  }

  /**
   * Sets the analog gain applied to the color channels.
   *
   * <p>The sensor only accepts gain changes while its integrator is off, so this briefly disables
   * and reenables the sensor, blocking for approximately 8 ms. If the sensor has not been
   * configured successfully, the gain is applied the next time configuration succeeds.
   *
   * @param gain color channel gain
   * @throws NullPointerException if {@code gain} is null
   * @throws IllegalStateException if this driver has been closed
   */
  public void setGain(Gain gain) {
    m_gain = ErrorMessages.requireNonNullParam(gain, "gain", "setGain");
    reconfigure();
  }

  /**
   * Returns the configured analog gain.
   *
   * @return color channel gain
   */
  public Gain getGain() {
    return m_gain;
  }

  /**
   * Sets the nominal drive current of the proximity LED.
   *
   * <p>The distance calibration returned by {@link #getDistanceMeters()} was fitted at {@link
   * LedDrive#PERCENT_50}, the default. Changing the drive current changes the reflected light level
   * for a given distance, so {@link #setDistanceCalibration(double, double, double)} must be used
   * to refit the calibration.
   *
   * <p>This briefly disables and reenables the sensor, blocking for approximately 8 ms. If the
   * sensor has not been configured successfully, the drive current is applied the next time
   * configuration succeeds.
   *
   * @param ledDrive proximity LED drive current
   * @throws NullPointerException if {@code ledDrive} is null
   * @throws IllegalStateException if this driver has been closed
   */
  public void setLedDrive(LedDrive ledDrive) {
    m_ledDrive = ErrorMessages.requireNonNullParam(ledDrive, "ledDrive", "setLedDrive");
    reconfigure();
  }

  /**
   * Returns the configured proximity LED drive current.
   *
   * @return proximity LED drive current
   */
  public LedDrive getLedDrive() {
    return m_ledDrive;
  }

  /**
   * Sets the color channel integration time.
   *
   * <p>The sensor integrates in 2.4 ms cycles, so the time is rounded up to the next whole cycle,
   * up to a maximum of 614.4 ms. A longer integration time raises {@link
   * #getMaximumRawColorValue()} and therefore the resolution of the color channels.
   *
   * <p>The sensor only accepts integration time changes while its integrator is off, so this
   * briefly disables and reenables the sensor, blocking for approximately 8 ms. If the sensor has
   * not been configured successfully, the integration time is applied the next time configuration
   * succeeds.
   *
   * @param integrationTime color channel integration time
   * @throws NullPointerException if {@code integrationTime} is null
   * @throws IllegalArgumentException if {@code integrationTime} is nonfinite or not positive
   * @throws IllegalStateException if this driver has been closed
   */
  public void setIntegrationTime(Time integrationTime) {
    ErrorMessages.requireNonNullParam(integrationTime, "integrationTime", "setIntegrationTime");
    m_integrationTimeRegister = integrationTimeRegister(integrationTime.in(Milliseconds));
    reconfigure();
  }

  /**
   * Returns the configured color channel integration time.
   *
   * @return color channel integration time
   */
  public Time getIntegrationTime() {
    return Milliseconds.of(integrationCycles() * INTEGRATION_CYCLE_MILLISECONDS);
  }

  /**
   * Sets the number of times the proximity LED is pulsed during each proximity cycle. More pulses
   * raise the reflected light level for a given distance.
   *
   * <p>The distance calibration returned by {@link #getDistanceMeters()} was fitted at the default
   * of 8 pulses, so changing the pulse count requires refitting the calibration with {@link
   * #setDistanceCalibration(double, double, double)}.
   *
   * <p>This briefly disables and reenables the sensor, blocking for approximately 8 ms. If the
   * sensor has not been configured successfully, the pulse count is applied the next time
   * configuration succeeds.
   *
   * @param proximityPulseCount number of LED pulses per proximity cycle, from 1 to 255
   * @throws IllegalArgumentException if {@code proximityPulseCount} is outside 1 to 255
   * @throws IllegalStateException if this driver has been closed
   */
  public void setProximityPulseCount(int proximityPulseCount) {
    if (proximityPulseCount < 1 || proximityPulseCount > 255) {
      throw new IllegalArgumentException("proximityPulseCount must be between 1 and 255");
    }
    m_proximityPulseCount = proximityPulseCount;
    reconfigure();
  }

  /**
   * Returns the configured number of proximity LED pulses per proximity cycle.
   *
   * @return number of LED pulses per proximity cycle
   */
  public int getProximityPulseCount() {
    return m_proximityPulseCount;
  }

  /**
   * Sets a scale factor applied by this driver to the normalized color channels. This is applied in
   * software after the sensor's own gain; it does not change the raw channel values.
   *
   * @param softwareGain scale factor applied to the normalized color channels
   * @throws IllegalArgumentException if {@code softwareGain} is nonfinite or not positive
   */
  public void setSoftwareGain(double softwareGain) {
    if (!Double.isFinite(softwareGain) || softwareGain <= 0.0) {
      throw new IllegalArgumentException("softwareGain must be finite and greater than zero");
    }
    m_softwareGain = softwareGain;
  }

  /**
   * Returns the scale factor applied by this driver to the normalized color channels.
   *
   * @return scale factor applied to the normalized color channels
   */
  public double getSoftwareGain() {
    return m_softwareGain;
  }

  /**
   * Sets the parameters of the curve that converts the raw proximity reading into a distance.
   *
   * <p>The raw proximity signal follows an inverse square law, and the parameters fit it to a
   * linear measure of distance in centimeters:
   *
   * <pre>rawProximity = a + b * (cm + c)^-2</pre>
   *
   * <p>The default parameters were fitted for this sensor at the default LED drive current and
   * proximity pulse count. Because the fit is affected by the infrared reflectivity of the target
   * surface, the linearity it produces is usually preserved on other surfaces even when the scale
   * is not, so a simple multiplicative correction is often enough to retune it.
   *
   * @param a the {@code a} parameter of the fitted curve
   * @param b the {@code b} parameter of the fitted curve
   * @param c the {@code c} parameter of the fitted curve
   * @throws IllegalArgumentException if any parameter is nonfinite
   */
  public void setDistanceCalibration(double a, double b, double c) {
    if (!Double.isFinite(a) || !Double.isFinite(b) || !Double.isFinite(c)) {
      throw new IllegalArgumentException("Distance calibration parameters must be finite");
    }
    m_aParam = a;
    m_bParam = b;
    m_cParam = c;
  }

  /**
   * Returns the distance to the target measured by the infrared proximity channel, from the plastic
   * housing at the front of the sensor.
   *
   * <p>Readings are most accurate perpendicular to the target surface; a cosine correction is
   * usually appropriate otherwise. The usable range is roughly 1 to 10 cm, and this returns {@link
   * Double#NaN} when the reflected light level is too low for the calibration curve, which is the
   * case when the target is beyond that range.
   *
   * @return distance in meters, or {@link Double#NaN} if the target is out of range
   * @throws IllegalStateException if this driver has been closed
   */
  public double getDistanceMeters() {
    requireOpen();
    if (m_rawProximity <= m_aParam) {
      return Double.NaN;
    }
    double centimeters =
        (-m_aParam * m_cParam
                + m_cParam * m_rawProximity
                - Math.sqrt(-m_aParam * m_bParam + m_bParam * m_rawProximity))
            / (m_aParam - m_rawProximity);
    return centimeters / 100.0;
  }

  /**
   * Returns the distance to the target measured by the infrared proximity channel, from the plastic
   * housing at the front of the sensor.
   *
   * @return distance, or a measure of {@link Double#NaN} if the target is out of range
   * @throws IllegalStateException if this driver has been closed
   * @see #getDistanceMeters()
   */
  public Distance getDistance() {
    return Meters.of(getDistanceMeters());
  }

  /**
   * Returns the raw reflected light level measured by the infrared proximity channel. The value
   * rises as the target gets closer.
   *
   * @return raw proximity reading, from 0 to {@link #getMaximumRawProximityValue()}
   * @throws IllegalStateException if this driver has been closed
   */
  public int getRawProximity() {
    requireOpen();
    return m_rawProximity;
  }

  /**
   * Returns the reflected light level measured by the infrared proximity channel, normalized
   * against its saturation value. The value rises as the target gets closer.
   *
   * @return proximity reading, from 0 to 1
   * @throws IllegalStateException if this driver has been closed
   */
  public double getProximity() {
    requireOpen();
    return Math.clamp(m_rawProximity / (double) PROXIMITY_SATURATION, 0.0, 1.0);
  }

  /**
   * Returns the raw proximity reading at which the proximity channel saturates.
   *
   * @return maximum raw proximity reading
   */
  public int getMaximumRawProximityValue() {
    return PROXIMITY_SATURATION;
  }

  /**
   * Returns the color measured by the sensor, with each channel normalized against {@link
   * #getMaximumRawColorValue()} and scaled by the software gain.
   *
   * @return measured color
   * @throws IllegalStateException if this driver has been closed
   */
  public Color getColor() {
    return new Color(getRed(), getGreen(), getBlue());
  }

  /**
   * Returns the normalized red channel.
   *
   * @return red channel, from 0 to 1
   * @throws IllegalStateException if this driver has been closed
   */
  public double getRed() {
    return normalize(getRawRed());
  }

  /**
   * Returns the normalized green channel.
   *
   * @return green channel, from 0 to 1
   * @throws IllegalStateException if this driver has been closed
   */
  public double getGreen() {
    return normalize(getRawGreen());
  }

  /**
   * Returns the normalized blue channel.
   *
   * @return blue channel, from 0 to 1
   * @throws IllegalStateException if this driver has been closed
   */
  public double getBlue() {
    return normalize(getRawBlue());
  }

  /**
   * Returns the normalized clear channel, which measures unfiltered light and is a useful measure
   * of overall brightness.
   *
   * @return clear channel, from 0 to 1
   * @throws IllegalStateException if this driver has been closed
   */
  public double getClear() {
    return normalize(getRawClear());
  }

  /**
   * Returns the raw red channel count.
   *
   * @return red channel, from 0 to {@link #getMaximumRawColorValue()}
   * @throws IllegalStateException if this driver has been closed
   */
  public int getRawRed() {
    requireOpen();
    return m_rawRed;
  }

  /**
   * Returns the raw green channel count.
   *
   * @return green channel, from 0 to {@link #getMaximumRawColorValue()}
   * @throws IllegalStateException if this driver has been closed
   */
  public int getRawGreen() {
    requireOpen();
    return m_rawGreen;
  }

  /**
   * Returns the raw blue channel count.
   *
   * @return blue channel, from 0 to {@link #getMaximumRawColorValue()}
   * @throws IllegalStateException if this driver has been closed
   */
  public int getRawBlue() {
    requireOpen();
    return m_rawBlue;
  }

  /**
   * Returns the raw clear channel count.
   *
   * @return clear channel, from 0 to {@link #getMaximumRawColorValue()}
   * @throws IllegalStateException if this driver has been closed
   */
  public int getRawClear() {
    requireOpen();
    return m_rawClear;
  }

  /**
   * Returns the highest raw color channel count reachable with the configured integration time.
   *
   * @return maximum raw color channel count
   */
  public int getMaximumRawColorValue() {
    return Math.min(MAX_RAW_COLOR_VALUE, COUNTS_PER_INTEGRATION_CYCLE * integrationCycles());
  }

  /**
   * Returns the device identifier reported by the sensor.
   *
   * @return device identifier, or zero if it has not been read successfully
   * @throws IllegalStateException if this driver has been closed
   */
  public byte getDeviceId() {
    requireOpen();
    return m_deviceId;
  }

  /**
   * Returns the current device or read status.
   *
   * @return device status
   * @throws IllegalStateException if this driver has been closed
   */
  public DeviceStatus getDeviceStatus() {
    requireOpen();
    return m_deviceStatus;
  }

  /**
   * Returns the reason for the most recent driver-detected failure.
   *
   * @return failure reason, or null if the driver has not detected a failure
   * @throws IllegalStateException if this driver has been closed
   */
  public FailureReason getLastFailureReason() {
    requireOpen();
    return m_lastFailureReason;
  }

  /**
   * Returns the total number of failures detected by this driver instance.
   *
   * @return total failure count
   * @throws IllegalStateException if this driver has been closed
   */
  public long getFailureCount() {
    requireOpen();
    return m_failureCount;
  }

  private I2C requireOpen() {
    if (m_i2c == null) {
      throw new IllegalStateException("RevColorSensorV2 has been closed");
    }
    return m_i2c;
  }

  private boolean initialize() {
    m_initialized = false;
    m_deviceStatus = DeviceStatus.NOT_INITIALIZED;

    byte[] data = readRegister(Register.DEVICE_ID, 1);
    if (data.length == 0) {
      return false;
    }
    m_deviceId = data[0];
    if (m_deviceId != TMD37821_DEVICE_ID && m_deviceId != TMD37823_DEVICE_ID) {
      recordFailure(FailureReason.UNEXPECTED_DEVICE_ID);
      return false;
    }
    return configure();
  }

  private void reconfigure() {
    requireOpen();
    if (m_initialized) {
      configure();
    }
  }

  /**
   * Writes the cached configuration to the sensor. The integrator must be off while the gain and
   * integration time are written, so the sensor is disabled first and reenabled afterwards.
   *
   * @return whether every transaction succeeded
   */
  private boolean configure() {
    m_initialized = false;
    return disable()
        && writeRegister(Register.ATIME, m_integrationTimeRegister)
        && writeControl()
        && writeRegister(Register.PPULSE, m_proximityPulseCount)
        && enable();
  }

  /**
   * Writes every CONTROL field explicitly rather than preserving the register's current contents.
   * Disabling the sensor does not reset CONTROL, so a value retained from an earlier configuration
   * could leave a reserved proximity diode or proximity gain selection in place, which changes
   * every proximity reading and invalidates the distance calibration.
   *
   * <p>This covers all eight bits: the LED drive occupies bits 7:6, the proximity diode select bits
   * 5:4, the proximity gain bits 3:2, and the color gain bits 1:0. The proximity gain is left as
   * zero, selecting the 1x gain the distance calibration was fitted at and the only value this part
   * defines.
   *
   * @return whether the write succeeded
   */
  private boolean writeControl() {
    int control = m_ledDrive.m_value | CONTROL_PDIODE_IR | m_gain.m_value;
    return writeRegister(Register.CONTROL, control);
  }

  private boolean enable() {
    if (!writeEnable(ENABLE_POWER_ON)) {
      return false;
    }
    delay(POWER_ON_DELAY_MILLISECONDS);

    if (!writeEnable(ENABLE_POWER_ON | ENABLE_COLOR | ENABLE_PROXIMITY)) {
      return false;
    }
    delay(ENABLE_SETTLE_DELAY_MILLISECONDS);

    m_initialized = true;
    m_deviceStatus = DeviceStatus.READY;
    return true;
  }

  private boolean disable() {
    return writeEnable(0);
  }

  private boolean writeEnable(int value) {
    // The interrupt enables are not used, and the reserved high bits must be written as zero.
    return writeRegister(Register.ENABLE, value & ENABLE_WRITABLE_MASK);
  }

  private byte[] readRegister(Register register, int count) {
    byte[] data = new byte[count];
    if (requireOpen().read(register.m_address | COMMAND_AUTO_INCREMENT, count, data)) {
      recordFailure(FailureReason.I2C_READ_ABORTED);
      return new byte[0];
    }
    return data;
  }

  private boolean writeRegister(Register register, int value) {
    if (requireOpen().write(register.m_address | COMMAND_AUTO_INCREMENT, value)) {
      recordFailure(FailureReason.I2C_WRITE_ABORTED);
      return false;
    }
    return true;
  }

  private void recordFailure(FailureReason reason) {
    m_lastFailureReason = reason;
    m_failureCount++;
    m_deviceStatus =
        reason == FailureReason.UNEXPECTED_DEVICE_ID
            ? DeviceStatus.FAULT_UNEXPECTED_DEVICE_ID
            : DeviceStatus.FAULT_BAD_READ;
  }

  private double normalize(int rawValue) {
    return Math.clamp(m_softwareGain * rawValue / getMaximumRawColorValue(), 0.0, 1.0);
  }

  private int integrationCycles() {
    return MAX_INTEGRATION_CYCLES - m_integrationTimeRegister;
  }

  /**
   * Returns the ATIME register value that integrates for at least the given duration. ATIME is the
   * two's complement of the number of 2.4 ms integration cycles.
   *
   * @param milliseconds requested integration time
   * @return ATIME register value
   */
  private static int integrationTimeRegister(double milliseconds) {
    if (!Double.isFinite(milliseconds) || milliseconds <= 0.0) {
      throw new IllegalArgumentException("integrationTime must be finite and greater than zero");
    }
    int cycles = (int) Math.ceil(milliseconds / INTEGRATION_CYCLE_MILLISECONDS);
    return MAX_INTEGRATION_CYCLES - Math.min(cycles, MAX_INTEGRATION_CYCLES);
  }

  private static int decodeUnsignedShort(byte[] data, int offset) {
    return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
  }

  private static void delay(int milliseconds) {
    try {
      Thread.sleep(milliseconds);
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
  }
}
