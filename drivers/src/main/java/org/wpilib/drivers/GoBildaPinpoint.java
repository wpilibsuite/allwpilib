// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//
// This file is based on the goBILDA FTC driver, which was made available under
// the following license:
//
// MIT License
//
// Copyright (c) 2025 Base 10 Assets, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

package org.wpilib.drivers;

import static org.wpilib.units.Units.Hertz;
import static org.wpilib.units.Units.Meters;
import static org.wpilib.units.Units.MetersPerSecond;
import static org.wpilib.units.Units.Millimeters;
import static org.wpilib.units.Units.Radians;
import static org.wpilib.units.Units.RadiansPerSecond;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.LinkedHashSet;
import org.wpilib.hardware.bus.I2C;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Quaternion;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.units.measure.Angle;
import org.wpilib.units.measure.AngularVelocity;
import org.wpilib.units.measure.Distance;
import org.wpilib.units.measure.Frequency;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.util.ErrorMessages;

/**
 * Driver for the goBILDA Pinpoint Odometry Computer.
 *
 * <p>Aborted I2C transactions and invalid read data are reported through {@link
 * DeviceStatus#FAULT_BAD_READ} instead of throwing an exception. Unless otherwise noted, methods
 * that access the device throw {@link IllegalStateException} after this driver is closed.
 */
public class GoBildaPinpoint implements AutoCloseable {
  /** Default 7-bit I2C address of the Pinpoint. */
  public static final int DEFAULT_ADDRESS = 0x31;

  private static final int REGISTER_LENGTH = 4;
  private static final int FIXED_BULK_READ_LENGTH = 40;
  private static final int CRC_LENGTH = 1;

  private static final byte CRC_INITIAL_VALUE = (byte) 0x90;
  private static final byte CRC_POLYNOMIAL_VALUE = 0x31;

  private static final double SWINGARM_TICKS_PER_METER = 13_262.91192;
  private static final double FOUR_BAR_TICKS_PER_METER = 19_894.36789;

  private static final float POSITION_CHANGE_LIMIT_MM = 5000.0f;
  private static final float HEADING_CHANGE_LIMIT_RADIANS = 120.0f;
  private static final float VELOCITY_LIMIT_MM_PER_SECOND = 10_000.0f;
  private static final float HEADING_VELOCITY_LIMIT_RADIANS_PER_SECOND = 120.0f;

  private static final Register[] DEFAULT_BULK_READ_SCOPE = {
    Register.DEVICE_STATUS,
    Register.LOOP_TIME,
    Register.X_ENCODER_VALUE,
    Register.Y_ENCODER_VALUE,
    Register.X_POSITION,
    Register.Y_POSITION,
    Register.H_ORIENTATION,
    Register.X_VELOCITY,
    Register.Y_VELOCITY,
    Register.H_VELOCITY
  };

  /** Error detection method used for I2C reads. */
  public enum ErrorDetectionType {
    /** Do not validate data read from the device. */
    NONE,
    /** Validate the CRC-8 byte returned by v3 or newer firmware. */
    CRC,
    /** Reject nonfinite values and implausibly large changes locally. */
    LOCAL_TEST
  }

  /** Reason the driver rejected data or an I2C transaction. */
  public enum FailureReason {
    /** The I2C controller aborted a read transaction. */
    I2C_READ_ABORTED,
    /** The I2C controller aborted a write transaction. */
    I2C_WRITE_ABORTED,
    /** Data returned by the device did not have the expected CRC-8 value. */
    CRC_MISMATCH,
    /** A bulk read returned a nonpositive device loop time. */
    INVALID_LOOP_TIME,
    /** A floating-point register returned a nonfinite value. */
    NONFINITE_VALUE,
    /** A position or orientation changed by more than the local validation limit. */
    CHANGE_TOO_LARGE,
    /** A velocity magnitude exceeded the local validation limit. */
    VELOCITY_TOO_LARGE
  }

  /** Register map for the Pinpoint. */
  public enum Register {
    /** Device identifier. */
    DEVICE_ID(1, RegisterType.INT32),
    /** Firmware version. */
    DEVICE_VERSION(2, RegisterType.INT32),
    /** Device status bit field. */
    DEVICE_STATUS(3, RegisterType.INT32),
    /** Device control command. */
    DEVICE_CONTROL(4, RegisterType.INT32),
    /** Device loop time. */
    LOOP_TIME(5, RegisterType.INT32),
    /** Raw X encoder value. */
    X_ENCODER_VALUE(6, RegisterType.INT32),
    /** Raw Y encoder value. */
    Y_ENCODER_VALUE(7, RegisterType.INT32),
    /** X position in millimeters. */
    X_POSITION(8, RegisterType.FLOAT),
    /** Y position in millimeters. */
    Y_POSITION(9, RegisterType.FLOAT),
    /** Heading in radians. */
    H_ORIENTATION(10, RegisterType.FLOAT),
    /** X velocity in millimeters per second. */
    X_VELOCITY(11, RegisterType.FLOAT),
    /** Y velocity in millimeters per second. */
    Y_VELOCITY(12, RegisterType.FLOAT),
    /** Heading velocity in radians per second. */
    H_VELOCITY(13, RegisterType.FLOAT),
    /** Millimeters traveled per encoder tick. */
    MM_PER_TICK(14, RegisterType.FLOAT),
    /** X pod offset in millimeters. */
    X_POD_OFFSET(15, RegisterType.FLOAT),
    /** Y pod offset in millimeters. */
    Y_POD_OFFSET(16, RegisterType.FLOAT),
    /** IMU yaw scalar. */
    YAW_SCALAR(17, RegisterType.FLOAT),
    /** Bulk-read window. */
    BULK_READ(18, RegisterType.BULK),
    /** Quaternion W component. */
    QUATERNION_W(19, RegisterType.FLOAT),
    /** Quaternion X component. */
    QUATERNION_X(20, RegisterType.FLOAT),
    /** Quaternion Y component. */
    QUATERNION_Y(21, RegisterType.FLOAT),
    /** Quaternion Z component. */
    QUATERNION_Z(22, RegisterType.FLOAT),
    /** Pitch in radians. */
    PITCH(23, RegisterType.FLOAT),
    /** Roll in radians. */
    ROLL(24, RegisterType.FLOAT),
    /** Flexible bulk-read configuration. */
    SET_BULK_READ(25, RegisterType.INT32);

    private final int m_address;
    private final RegisterType m_type;

    Register(int address, RegisterType type) {
      m_address = address;
      m_type = type;
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

  /** Pinpoint status, including device-reported faults and local read failures. */
  public enum DeviceStatus {
    /** The device is starting up and has not initialized. */
    NOT_READY(0),
    /** The device is operating normally. */
    READY(1),
    /** The device is calibrating its IMU. */
    CALIBRATING(1 << 1),
    /** The X odometry pod was not detected. */
    FAULT_X_POD_NOT_DETECTED(1 << 2),
    /** The Y odometry pod was not detected. */
    FAULT_Y_POD_NOT_DETECTED(1 << 3),
    /** Neither odometry pod was detected. */
    FAULT_NO_PODS_DETECTED((1 << 2) | (1 << 3)),
    /** The IMU reported a runaway condition. */
    FAULT_IMU_RUNAWAY(1 << 4),
    /** A read failed validation or the I2C transaction was aborted. */
    FAULT_BAD_READ(1 << 5);

    private final int m_mask;

    DeviceStatus(int mask) {
      m_mask = mask;
    }
  }

  /** Encoder direction relative to the Pinpoint coordinate system. */
  public enum EncoderDirection {
    /** Encoder count increases in the positive axis direction. */
    FORWARD,
    /** Encoder count decreases in the positive axis direction. */
    REVERSED
  }

  /** goBILDA odometry pod with a predefined encoder resolution. */
  public enum OdometryPod {
    /** goBILDA Swingarm Odometry Pod. */
    SWINGARM(SWINGARM_TICKS_PER_METER),
    /** goBILDA 4-Bar Odometry Pod. */
    FOUR_BAR(FOUR_BAR_TICKS_PER_METER);

    private final double m_ticksPerMeter;

    OdometryPod(double ticksPerMeter) {
      m_ticksPerMeter = ticksPerMeter;
    }
  }

  private enum RegisterType {
    INT32,
    FLOAT,
    BULK
  }

  private enum DeviceControl {
    RECALIBRATE_IMU(1 << 0),
    RESET_POSITION_AND_IMU(1 << 1),
    SET_Y_ENCODER_REVERSED(1 << 2),
    SET_Y_ENCODER_FORWARD(1 << 3),
    SET_X_ENCODER_REVERSED(1 << 4),
    SET_X_ENCODER_FORWARD(1 << 5);

    private final int m_value;

    DeviceControl(int value) {
      m_value = value;
    }
  }

  private I2C m_i2c;
  private Register[] m_bulkReadScope = DEFAULT_BULK_READ_SCOPE.clone();
  private ErrorDetectionType m_errorDetectionType = ErrorDetectionType.LOCAL_TEST;

  private int m_deviceId;
  private int m_deviceVersion;
  private int m_deviceStatus;
  private int m_loopTimeMicroseconds;
  private int m_xEncoderValue;
  private int m_yEncoderValue;
  private float m_xPositionMillimeters;
  private float m_yPositionMillimeters;
  private float m_headingRadians;
  private float m_xVelocityMillimetersPerSecond;
  private float m_yVelocityMillimetersPerSecond;
  private float m_headingVelocityRadiansPerSecond;
  private float m_millimetersPerTick;
  private float m_xPodOffsetMillimeters;
  private float m_yPodOffsetMillimeters;
  private float m_yawScalar;
  private float m_quaternionW;
  private float m_quaternionX;
  private float m_quaternionY;
  private float m_quaternionZ;
  private float m_pitchRadians;
  private float m_rollRadians;

  private boolean m_haveXPosition;
  private boolean m_haveYPosition;
  private boolean m_haveHeading;
  private boolean m_badReadDetected;
  private final long[] m_failureCounts = new long[Register.values().length];
  private long m_failureCount;
  private Register m_lastFailedRegister;
  private FailureReason m_lastFailureReason;

  /**
   * Constructs a Pinpoint on its default I2C address.
   *
   * @param port I2C port to which the Pinpoint is connected
   * @throws NullPointerException if {@code port} is null
   */
  public GoBildaPinpoint(I2C.Port port) {
    this(port, DEFAULT_ADDRESS);
  }

  /**
   * Constructs a Pinpoint.
   *
   * @param port I2C port to which the Pinpoint is connected
   * @param deviceAddress 7-bit I2C address
   * @throws NullPointerException if {@code port} is null
   * @throws IllegalArgumentException if {@code deviceAddress} is outside the 7-bit address range
   */
  public GoBildaPinpoint(I2C.Port port, int deviceAddress) {
    ErrorMessages.requireNonNullParam(port, "port", "GoBildaPinpoint");
    if (deviceAddress < 0 || deviceAddress > 0x7f) {
      throw new IllegalArgumentException("deviceAddress must be a 7-bit I2C address");
    }

    m_i2c = new I2C(port, deviceAddress);
    HAL.reportUsage("I2C[" + port.value + "][" + deviceAddress + "]", "GoBildaPinpoint");
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
   * Reads all registers in the configured bulk-read scope and updates the cached measurements. Call
   * this once per robot loop.
   *
   * <p>An aborted or invalid I2C read does not throw; it sets the device status to {@link
   * DeviceStatus#FAULT_BAD_READ} and preserves the last valid measurements.
   *
   * @throws IllegalStateException if this driver has been closed
   * @throws UnsupportedOperationException if CRC error detection is active with firmware older than
   *     v3
   */
  public void update() {
    requireOpen();
    if (m_deviceVersion == 0) {
      readRegister(Register.DEVICE_VERSION);
    }

    if (m_deviceVersion == 1 || m_deviceVersion == 2) {
      fixedBulkRead();
    } else if (m_deviceVersion >= 3) {
      flexibleBulkRead();
    }
  }

  /**
   * Reads and caches only the heading register.
   *
   * <p>An aborted or invalid I2C read does not throw; it sets the device status to {@link
   * DeviceStatus#FAULT_BAD_READ} and preserves the last valid heading.
   *
   * @throws IllegalStateException if this driver has been closed
   */
  public void updateHeading() {
    requireOpen();
    final int previousStatus = m_deviceStatus;
    readRegister(Register.H_ORIENTATION);
    if (!m_badReadDetected && decodeStatus(previousStatus) == DeviceStatus.FAULT_BAD_READ) {
      m_deviceStatus = DeviceStatus.READY.m_mask;
    }
  }

  /**
   * Configures the registers returned by a bulk read on v3 or newer firmware. Duplicate registers
   * are ignored while preserving the order of their first occurrence.
   *
   * @param registers readable registers to include
   * @throws NullPointerException if {@code registers} or any register in it is null
   * @throws UnsupportedOperationException if the firmware is older than v3
   * @throws IllegalArgumentException if no registers are provided or a register is not readable
   * @throws IllegalStateException if this driver has been closed
   */
  public void setBulkReadScope(Register... registers) {
    ErrorMessages.requireNonNullParam(registers, "registers", "setBulkReadScope");
    requireFirmwareVersion3("Flexible bulk reads");

    var uniqueRegisters = new LinkedHashSet<Register>();
    for (Register register : registers) {
      ErrorMessages.requireNonNullParam(register, "registers contains null", "setBulkReadScope");
      if (!isIndividuallyReadable(register)) {
        throw new IllegalArgumentException(register + " is not a readable data register");
      }
      uniqueRegisters.add(register);
    }
    if (uniqueRegisters.isEmpty()) {
      throw new IllegalArgumentException("At least one bulk-read register is required");
    }

    Register[] scope = uniqueRegisters.toArray(Register[]::new);
    byte[] addresses = new byte[scope.length];
    for (int i = 0; i < scope.length; i++) {
      addresses[i] = (byte) scope[i].m_address;
    }

    if (writeBytes(Register.SET_BULK_READ, addresses)) {
      m_bulkReadScope = scope;
    }
  }

  /**
   * Selects the read error detection method. Selecting CRC reads the firmware version and requires
   * v3 or newer firmware.
   *
   * @param errorDetectionType error detection method
   * @throws NullPointerException if {@code errorDetectionType} is null
   * @throws UnsupportedOperationException if CRC is selected with firmware older than v3
   * @throws IllegalStateException if this driver has been closed
   */
  public void setErrorDetectionType(ErrorDetectionType errorDetectionType) {
    ErrorMessages.requireNonNullParam(
        errorDetectionType, "errorDetectionType", "setErrorDetectionType");
    requireOpen();
    if (errorDetectionType == ErrorDetectionType.CRC) {
      requireFirmwareVersion3("CRC error detection");
    }
    m_errorDetectionType = errorDetectionType;
  }

  /**
   * Sets the odometry pod offsets in meters.
   *
   * <p>The X pod offset is positive to the left of the tracking point. The Y pod offset is positive
   * in front of the tracking point.
   *
   * @param xOffsetMeters X pod offset in meters
   * @param yOffsetMeters Y pod offset in meters
   * @throws IllegalArgumentException if either offset is nonfinite or cannot be represented by the
   *     device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setOffsets(double xOffsetMeters, double yOffsetMeters) {
    writeFloat(Register.X_POD_OFFSET, metersToMillimeters(xOffsetMeters, "xOffsetMeters"));
    writeFloat(Register.Y_POD_OFFSET, metersToMillimeters(yOffsetMeters, "yOffsetMeters"));
  }

  /**
   * Sets the odometry pod offsets.
   *
   * <p>The X pod offset is positive to the left of the tracking point. The Y pod offset is positive
   * in front of the tracking point.
   *
   * @param xOffset X pod offset
   * @param yOffset Y pod offset
   * @throws NullPointerException if either offset is null
   * @throws IllegalArgumentException if either converted offset is nonfinite or cannot be
   *     represented by the device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setOffsets(Distance xOffset, Distance yOffset) {
    ErrorMessages.requireNonNullParam(xOffset, "xOffset", "setOffsets");
    ErrorMessages.requireNonNullParam(yOffset, "yOffset", "setOffsets");
    writeFloat(
        Register.X_POD_OFFSET,
        requireFiniteFloat(xOffset.in(Millimeters), "xOffset in millimeters"));
    writeFloat(
        Register.Y_POD_OFFSET,
        requireFiniteFloat(yOffset.in(Millimeters), "yOffset in millimeters"));
  }

  /**
   * Recalibrates the IMU. The robot must remain stationary for approximately 0.25 seconds.
   *
   * @throws IllegalStateException if this driver has been closed
   */
  public void recalibrateImu() {
    writeInt(Register.DEVICE_CONTROL, DeviceControl.RECALIBRATE_IMU.m_value);
  }

  /**
   * Resets the pose to the origin and recalibrates the IMU. The robot must remain stationary.
   *
   * @throws IllegalStateException if this driver has been closed
   */
  public void resetPositionAndImu() {
    writeInt(Register.DEVICE_CONTROL, DeviceControl.RESET_POSITION_AND_IMU.m_value);
  }

  /**
   * Sets the directions of both odometry pod encoders.
   *
   * @param xEncoder X (forward) pod direction
   * @param yEncoder Y (left) pod direction
   * @throws NullPointerException if either direction is null
   * @throws IllegalStateException if this driver has been closed
   */
  public void setEncoderDirections(EncoderDirection xEncoder, EncoderDirection yEncoder) {
    ErrorMessages.requireNonNullParam(xEncoder, "xEncoder", "setEncoderDirections");
    ErrorMessages.requireNonNullParam(yEncoder, "yEncoder", "setEncoderDirections");

    writeInt(
        Register.DEVICE_CONTROL,
        xEncoder == EncoderDirection.FORWARD
            ? DeviceControl.SET_X_ENCODER_FORWARD.m_value
            : DeviceControl.SET_X_ENCODER_REVERSED.m_value);
    writeInt(
        Register.DEVICE_CONTROL,
        yEncoder == EncoderDirection.FORWARD
            ? DeviceControl.SET_Y_ENCODER_FORWARD.m_value
            : DeviceControl.SET_Y_ENCODER_REVERSED.m_value);
  }

  /**
   * Sets the encoder resolution for a goBILDA odometry pod.
   *
   * @param pod odometry pod type
   * @throws NullPointerException if {@code pod} is null
   * @throws IllegalStateException if this driver has been closed
   */
  public void setEncoderResolution(OdometryPod pod) {
    setEncoderResolution(
        ErrorMessages.requireNonNullParam(pod, "pod", "setEncoderResolution").m_ticksPerMeter);
  }

  /**
   * Sets a custom odometry pod encoder resolution.
   *
   * @param ticksPerMeter encoder ticks per meter of pod travel
   * @throws IllegalArgumentException if {@code ticksPerMeter} is nonfinite, not positive, or
   *     produces a millimeters-per-tick value that cannot be represented by the device's 32-bit
   *     floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setEncoderResolution(double ticksPerMeter) {
    if (!Double.isFinite(ticksPerMeter) || ticksPerMeter <= 0.0) {
      throw new IllegalArgumentException("ticksPerMeter must be finite and greater than zero");
    }
    writeFloat(
        Register.MM_PER_TICK, requireFiniteFloat(1000.0 / ticksPerMeter, "millimeters per tick"));
  }

  /**
   * Sets the scalar applied to the IMU yaw measurement.
   *
   * @param yawScalar yaw scalar
   * @throws IllegalArgumentException if {@code yawScalar} is nonfinite or cannot be represented by
   *     the device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setYawScalar(double yawScalar) {
    writeFloat(Register.YAW_SCALAR, requireFiniteFloat(yawScalar, "yawScalar"));
  }

  /**
   * Overrides the pose tracked by the Pinpoint.
   *
   * @param pose pose in meters and radians
   * @throws NullPointerException if {@code pose} is null
   * @throws IllegalArgumentException if a pose component is nonfinite or cannot be represented by
   *     the device's 32-bit floating-point registers
   * @throws IllegalStateException if this driver has been closed
   */
  public void setPose(Pose2d pose) {
    ErrorMessages.requireNonNullParam(pose, "pose", "setPose");
    setXPosition(pose.getMeasureX());
    setYPosition(pose.getMeasureY());
    setHeading(pose.getRotation().getMeasure());
  }

  /**
   * Overrides the tracked X position.
   *
   * @param positionMeters X position in meters
   * @throws IllegalArgumentException if {@code positionMeters} is nonfinite or cannot be
   *     represented by the device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setXPositionMeters(double positionMeters) {
    writeFloat(Register.X_POSITION, metersToMillimeters(positionMeters, "positionMeters"));
  }

  /**
   * Overrides the tracked X position.
   *
   * @param position X position
   * @throws NullPointerException if {@code position} is null
   * @throws IllegalArgumentException if {@code position} is nonfinite or cannot be represented by
   *     the device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setXPosition(Distance position) {
    ErrorMessages.requireNonNullParam(position, "position", "setXPosition");
    setXPositionMeters(position.in(Meters));
  }

  /**
   * Overrides the tracked Y position.
   *
   * @param positionMeters Y position in meters
   * @throws IllegalArgumentException if {@code positionMeters} is nonfinite or cannot be
   *     represented by the device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setYPositionMeters(double positionMeters) {
    writeFloat(Register.Y_POSITION, metersToMillimeters(positionMeters, "positionMeters"));
  }

  /**
   * Overrides the tracked Y position.
   *
   * @param position Y position
   * @throws NullPointerException if {@code position} is null
   * @throws IllegalArgumentException if {@code position} is nonfinite or cannot be represented by
   *     the device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setYPosition(Distance position) {
    ErrorMessages.requireNonNullParam(position, "position", "setYPosition");
    setYPositionMeters(position.in(Meters));
  }

  /**
   * Overrides the tracked heading.
   *
   * @param headingRadians heading in radians
   * @throws IllegalArgumentException if {@code headingRadians} is nonfinite or cannot be
   *     represented by the device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setHeadingRadians(double headingRadians) {
    writeFloat(Register.H_ORIENTATION, requireFiniteFloat(headingRadians, "headingRadians"));
  }

  /**
   * Overrides the tracked heading.
   *
   * @param heading heading
   * @throws NullPointerException if {@code heading} is null
   * @throws IllegalArgumentException if {@code heading} is nonfinite or cannot be represented by
   *     the device's 32-bit floating-point register
   * @throws IllegalStateException if this driver has been closed
   */
  public void setHeading(Angle heading) {
    ErrorMessages.requireNonNullParam(heading, "heading", "setHeading");
    setHeadingRadians(heading.in(Radians));
  }

  /**
   * Returns the device identifier.
   *
   * @return device identifier
   * @throws IllegalStateException if this driver has been closed
   */
  public int getDeviceId() {
    readIfNotInBulkScope(Register.DEVICE_ID);
    return m_deviceId;
  }

  /**
   * Returns the firmware version.
   *
   * @return firmware version
   * @throws IllegalStateException if this driver has been closed
   */
  public int getDeviceVersion() {
    readIfNotInBulkScope(Register.DEVICE_VERSION);
    return m_deviceVersion;
  }

  /**
   * Returns the configured yaw scalar.
   *
   * @return yaw scalar
   * @throws IllegalStateException if this driver has been closed
   */
  public double getYawScalar() {
    readIfNotInBulkScope(Register.YAW_SCALAR);
    return m_yawScalar;
  }

  /**
   * Returns the configured encoder resolution.
   *
   * @return encoder ticks per meter, or positive infinity if the device reports zero millimeters
   *     per tick
   * @throws IllegalStateException if this driver has been closed
   */
  public double getEncoderResolutionTicksPerMeter() {
    readIfNotInBulkScope(Register.MM_PER_TICK);
    return 1000.0 / m_millimetersPerTick;
  }

  /**
   * Returns the highest-priority current device or read status.
   *
   * @return device status
   * @throws IllegalStateException if this driver has been closed
   */
  public DeviceStatus getDeviceStatus() {
    requireOpen();
    return decodeStatus(m_deviceStatus);
  }

  /**
   * Returns the raw device status bit field.
   *
   * @return status bits
   * @throws IllegalStateException if this driver has been closed
   */
  public int getDeviceStatusBits() {
    requireOpen();
    return m_deviceStatus;
  }

  /**
   * Returns the register associated with the most recent driver-detected failure.
   *
   * @return failed register, or null if the driver has not detected a failure
   * @throws IllegalStateException if this driver has been closed
   */
  public Register getLastFailedRegister() {
    requireOpen();
    return m_lastFailedRegister;
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

  /**
   * Returns the number of failures associated with a register.
   *
   * @param register register whose failure count should be returned
   * @return failure count for the register
   * @throws NullPointerException if {@code register} is null
   * @throws IllegalStateException if this driver has been closed
   */
  public long getFailureCount(Register register) {
    ErrorMessages.requireNonNullParam(register, "register", "getFailureCount");
    requireOpen();
    return m_failureCounts[register.ordinal()];
  }

  /**
   * Returns the most recently reported device loop time.
   *
   * @return loop time in microseconds
   * @throws IllegalStateException if this driver has been closed
   */
  public int getLoopTimeMicroseconds() {
    readIfNotInBulkScope(Register.LOOP_TIME);
    return m_loopTimeMicroseconds;
  }

  /**
   * Returns the most recently reported device loop frequency.
   *
   * @return frequency in hertz, or zero when no loop time has been read
   * @throws IllegalStateException if this driver has been closed
   */
  public double getFrequencyHz() {
    int loopTime = getLoopTimeMicroseconds();
    return loopTime == 0 ? 0.0 : 1_000_000.0 / loopTime;
  }

  /**
   * Returns the most recently reported device loop frequency.
   *
   * @return loop frequency, or zero when no loop time has been read
   * @throws IllegalStateException if this driver has been closed
   */
  public Frequency getFrequency() {
    return Hertz.of(getFrequencyHz());
  }

  /**
   * Returns the raw X encoder count.
   *
   * @return X encoder ticks
   * @throws IllegalStateException if this driver has been closed
   */
  public int getXEncoder() {
    readIfNotInBulkScope(Register.X_ENCODER_VALUE);
    return m_xEncoderValue;
  }

  /**
   * Returns the raw Y encoder count.
   *
   * @return Y encoder ticks
   * @throws IllegalStateException if this driver has been closed
   */
  public int getYEncoder() {
    readIfNotInBulkScope(Register.Y_ENCODER_VALUE);
    return m_yEncoderValue;
  }

  /**
   * Returns the tracked X position.
   *
   * @return X position in meters
   * @throws IllegalStateException if this driver has been closed
   */
  public double getXPositionMeters() {
    readIfNotInBulkScope(Register.X_POSITION);
    return m_xPositionMillimeters / 1000.0;
  }

  /**
   * Returns the tracked X position.
   *
   * @return X position
   * @throws IllegalStateException if this driver has been closed
   */
  public Distance getXPosition() {
    return Meters.of(getXPositionMeters());
  }

  /**
   * Returns the tracked Y position.
   *
   * @return Y position in meters
   * @throws IllegalStateException if this driver has been closed
   */
  public double getYPositionMeters() {
    readIfNotInBulkScope(Register.Y_POSITION);
    return m_yPositionMillimeters / 1000.0;
  }

  /**
   * Returns the tracked Y position.
   *
   * @return Y position
   * @throws IllegalStateException if this driver has been closed
   */
  public Distance getYPosition() {
    return Meters.of(getYPositionMeters());
  }

  /**
   * Returns the continuous tracked heading.
   *
   * @return heading in radians, not constrained to one rotation
   * @throws IllegalStateException if this driver has been closed
   */
  public double getHeadingRadians() {
    readIfNotInBulkScope(Register.H_ORIENTATION);
    return m_headingRadians;
  }

  /**
   * Returns the continuous tracked heading.
   *
   * @return heading, not constrained to one rotation
   * @throws IllegalStateException if this driver has been closed
   */
  public Angle getHeading() {
    return Radians.of(getHeadingRadians());
  }

  /**
   * Returns the tracked X velocity.
   *
   * @return X velocity in meters per second
   * @throws IllegalStateException if this driver has been closed
   */
  public double getXVelocityMetersPerSecond() {
    readIfNotInBulkScope(Register.X_VELOCITY);
    return m_xVelocityMillimetersPerSecond / 1000.0;
  }

  /**
   * Returns the tracked X velocity.
   *
   * @return X velocity
   * @throws IllegalStateException if this driver has been closed
   */
  public LinearVelocity getXVelocity() {
    return MetersPerSecond.of(getXVelocityMetersPerSecond());
  }

  /**
   * Returns the tracked Y velocity.
   *
   * @return Y velocity in meters per second
   * @throws IllegalStateException if this driver has been closed
   */
  public double getYVelocityMetersPerSecond() {
    readIfNotInBulkScope(Register.Y_VELOCITY);
    return m_yVelocityMillimetersPerSecond / 1000.0;
  }

  /**
   * Returns the tracked Y velocity.
   *
   * @return Y velocity
   * @throws IllegalStateException if this driver has been closed
   */
  public LinearVelocity getYVelocity() {
    return MetersPerSecond.of(getYVelocityMetersPerSecond());
  }

  /**
   * Returns the heading velocity.
   *
   * @return heading velocity in radians per second
   * @throws IllegalStateException if this driver has been closed
   */
  public double getHeadingVelocityRadiansPerSecond() {
    readIfNotInBulkScope(Register.H_VELOCITY);
    return m_headingVelocityRadiansPerSecond;
  }

  /**
   * Returns the heading velocity.
   *
   * @return heading velocity
   * @throws IllegalStateException if this driver has been closed
   */
  public AngularVelocity getHeadingVelocity() {
    return RadiansPerSecond.of(getHeadingVelocityRadiansPerSecond());
  }

  /**
   * Returns the X pod offset.
   *
   * @return X pod offset in meters
   * @throws IllegalStateException if this driver has been closed
   */
  public double getXOffsetMeters() {
    readIfNotInBulkScope(Register.X_POD_OFFSET);
    return m_xPodOffsetMillimeters / 1000.0;
  }

  /**
   * Returns the X pod offset.
   *
   * @return X pod offset
   * @throws IllegalStateException if this driver has been closed
   */
  public Distance getXOffset() {
    return Meters.of(getXOffsetMeters());
  }

  /**
   * Returns the Y pod offset.
   *
   * @return Y pod offset in meters
   * @throws IllegalStateException if this driver has been closed
   */
  public double getYOffsetMeters() {
    readIfNotInBulkScope(Register.Y_POD_OFFSET);
    return m_yPodOffsetMillimeters / 1000.0;
  }

  /**
   * Returns the Y pod offset.
   *
   * @return Y pod offset
   * @throws IllegalStateException if this driver has been closed
   */
  public Distance getYOffset() {
    return Meters.of(getYOffsetMeters());
  }

  /**
   * Returns the tracked pose. Translation is in meters; heading is normalized by {@link
   * Rotation2d}.
   *
   * @return tracked pose
   * @throws IllegalStateException if this driver has been closed
   */
  public Pose2d getPose() {
    readIfNotInBulkScope(Register.X_POSITION);
    readIfNotInBulkScope(Register.Y_POSITION);
    readIfNotInBulkScope(Register.H_ORIENTATION);
    return new Pose2d(
        m_xPositionMillimeters / 1000.0,
        m_yPositionMillimeters / 1000.0,
        new Rotation2d(m_headingRadians));
  }

  /**
   * Returns the device orientation quaternion. Requires v3 or newer firmware.
   *
   * @return orientation quaternion
   * @throws IllegalStateException if this driver has been closed
   * @throws UnsupportedOperationException if the firmware is older than v3
   */
  public Quaternion getQuaternion() {
    requireFirmwareVersion3("Quaternion output");
    readIfNotInBulkScope(Register.QUATERNION_W);
    readIfNotInBulkScope(Register.QUATERNION_X);
    readIfNotInBulkScope(Register.QUATERNION_Y);
    readIfNotInBulkScope(Register.QUATERNION_Z);
    return new Quaternion(m_quaternionW, m_quaternionX, m_quaternionY, m_quaternionZ);
  }

  /**
   * Returns the device orientation as a 3D rotation. Requires v3 or newer firmware.
   *
   * @return 3D orientation
   * @throws IllegalStateException if this driver has been closed
   * @throws UnsupportedOperationException if the firmware is older than v3
   */
  public Rotation3d getRotation3d() {
    return new Rotation3d(getQuaternion());
  }

  /**
   * Returns the pitch. Requires v3 or newer firmware.
   *
   * @return pitch in radians
   * @throws IllegalStateException if this driver has been closed
   * @throws UnsupportedOperationException if the firmware is older than v3
   */
  public double getPitchRadians() {
    requireFirmwareVersion3("Pitch output");
    readIfNotInBulkScope(Register.PITCH);
    return m_pitchRadians;
  }

  /**
   * Returns the pitch. Requires v3 or newer firmware.
   *
   * @return pitch
   * @throws IllegalStateException if this driver has been closed
   * @throws UnsupportedOperationException if the firmware is older than v3
   */
  public Angle getPitch() {
    return Radians.of(getPitchRadians());
  }

  /**
   * Returns the roll. Requires v3 or newer firmware.
   *
   * @return roll in radians
   * @throws IllegalStateException if this driver has been closed
   * @throws UnsupportedOperationException if the firmware is older than v3
   */
  public double getRollRadians() {
    requireFirmwareVersion3("Roll output");
    readIfNotInBulkScope(Register.ROLL);
    return m_rollRadians;
  }

  /**
   * Returns the roll. Requires v3 or newer firmware.
   *
   * @return roll
   * @throws IllegalStateException if this driver has been closed
   * @throws UnsupportedOperationException if the firmware is older than v3
   */
  public Angle getRoll() {
    return Radians.of(getRollRadians());
  }

  private I2C requireOpen() {
    if (m_i2c == null) {
      throw new IllegalStateException("GoBildaPinpoint has been closed");
    }
    return m_i2c;
  }

  private static float metersToMillimeters(double meters, String parameterName) {
    return requireFiniteFloat(meters * 1000.0, parameterName);
  }

  private static float requireFiniteFloat(double value, String parameterName) {
    float floatValue = (float) value;
    if (!Float.isFinite(floatValue)) {
      throw new IllegalArgumentException(
          parameterName + " must be finite and representable as a 32-bit float");
    }
    return floatValue;
  }

  private void requireFirmwareVersion3(String feature) {
    if (m_deviceVersion == 0) {
      readRegister(Register.DEVICE_VERSION);
    }
    if (m_deviceVersion < 3) {
      throw new UnsupportedOperationException(feature + " requires firmware version 3 or newer");
    }
  }

  private void readIfNotInBulkScope(Register register) {
    requireOpen();
    if (!Arrays.asList(m_bulkReadScope).contains(register)) {
      readRegister(register);
    }
  }

  private static boolean isIndividuallyReadable(Register register) {
    return register != Register.DEVICE_CONTROL
        && register != Register.BULK_READ
        && register != Register.SET_BULK_READ;
  }

  private boolean writeInt(Register register, int value) {
    return writeBytes(register, encodeInt(value));
  }

  private boolean writeFloat(Register register, float value) {
    return writeBytes(register, encodeFloat(value));
  }

  private boolean writeBytes(Register register, byte[] data) {
    byte[] output = new byte[data.length + 1];
    output[0] = (byte) register.m_address;
    System.arraycopy(data, 0, output, 1, data.length);
    if (requireOpen().writeBulk(output)) {
      recordFailure(register, FailureReason.I2C_WRITE_ABORTED);
      return false;
    }
    return true;
  }

  private byte[] readBytes(Register register, int count) {
    I2C i2c = requireOpen();
    if (i2c.writeBulk(new byte[] {(byte) register.m_address})) {
      recordFailure(register, FailureReason.I2C_WRITE_ABORTED);
      return new byte[0];
    }

    byte[] data = new byte[count];
    if (i2c.readOnly(data, count)) {
      recordFailure(register, FailureReason.I2C_READ_ABORTED);
      return new byte[0];
    }
    return data;
  }

  private void readRegister(Register register) {
    if (!isIndividuallyReadable(register)) {
      throw new IllegalArgumentException(register + " cannot be read individually");
    }

    final int previousStatus = m_deviceStatus;
    m_badReadDetected = false;
    int readLength = REGISTER_LENGTH;
    if (m_errorDetectionType == ErrorDetectionType.CRC) {
      readLength += CRC_LENGTH;
    }
    byte[] data = readBytes(register, readLength);
    if (data.length == 0) {
      return;
    }
    if (m_errorDetectionType == ErrorDetectionType.CRC && !checkCrc(data, REGISTER_LENGTH)) {
      recordFailure(register, FailureReason.CRC_MISMATCH);
      return;
    }

    switch (register.m_type) {
      case INT32 -> saveInt(register, decodeInt(data, 0));
      case FLOAT -> saveFloat(register, decodeFloat(data, 0), false);
      case BULK -> throw new IllegalArgumentException(register + " cannot be read individually");
      default -> throw new IllegalStateException("Unknown register type");
    }
    finishRead(previousStatus, register == Register.DEVICE_STATUS, false);
  }

  private void fixedBulkRead() {
    if (m_errorDetectionType == ErrorDetectionType.CRC) {
      throw new UnsupportedOperationException(
          "CRC error detection requires firmware version 3 or newer");
    }

    final int previousStatus = m_deviceStatus;
    byte[] data = readBytes(Register.BULK_READ, FIXED_BULK_READ_LENGTH);
    if (data.length == 0) {
      return;
    }

    int loopTime = decodeInt(data, 4);
    if (m_errorDetectionType == ErrorDetectionType.LOCAL_TEST && loopTime <= 0) {
      recordFailure(Register.LOOP_TIME, FailureReason.INVALID_LOOP_TIME);
      return;
    }

    m_badReadDetected = false;
    saveInt(Register.DEVICE_STATUS, decodeInt(data, 0));
    saveInt(Register.LOOP_TIME, loopTime);
    saveInt(Register.X_ENCODER_VALUE, decodeInt(data, 8));
    saveInt(Register.Y_ENCODER_VALUE, decodeInt(data, 12));
    saveFloat(Register.X_POSITION, decodeFloat(data, 16), true);
    saveFloat(Register.Y_POSITION, decodeFloat(data, 20), true);
    saveFloat(Register.H_ORIENTATION, decodeFloat(data, 24), true);
    saveFloat(Register.X_VELOCITY, decodeFloat(data, 28), true);
    saveFloat(Register.Y_VELOCITY, decodeFloat(data, 32), true);
    saveFloat(Register.H_VELOCITY, decodeFloat(data, 36), true);
    finishRead(previousStatus, true, true);
  }

  private void flexibleBulkRead() {
    final int previousStatus = m_deviceStatus;
    int dataLength = m_bulkReadScope.length * REGISTER_LENGTH;
    int readLength = dataLength + (m_errorDetectionType == ErrorDetectionType.CRC ? CRC_LENGTH : 0);
    byte[] data = readBytes(Register.BULK_READ, readLength);
    if (data.length == 0) {
      return;
    }
    if (m_errorDetectionType == ErrorDetectionType.CRC && !checkCrc(data, dataLength)) {
      recordFailure(Register.BULK_READ, FailureReason.CRC_MISMATCH);
      return;
    }

    m_badReadDetected = false;
    for (int i = 0; i < m_bulkReadScope.length; i++) {
      Register register = m_bulkReadScope[i];
      int offset = i * REGISTER_LENGTH;
      switch (register.m_type) {
        case INT32 -> saveInt(register, decodeInt(data, offset));
        case FLOAT -> saveFloat(register, decodeFloat(data, offset), false);
        case BULK -> throw new IllegalStateException("A bulk-read scope contains BULK_READ");
        default -> throw new IllegalStateException("Unknown register type");
      }
    }
    finishRead(
        previousStatus, Arrays.asList(m_bulkReadScope).contains(Register.DEVICE_STATUS), true);
  }

  private void saveInt(Register register, int value) {
    switch (register) {
      case DEVICE_ID -> m_deviceId = value;
      case DEVICE_VERSION -> m_deviceVersion = value;
      case DEVICE_STATUS -> m_deviceStatus = value;
      case LOOP_TIME -> m_loopTimeMicroseconds = value;
      case X_ENCODER_VALUE -> m_xEncoderValue = value;
      case Y_ENCODER_VALUE -> m_yEncoderValue = value;
      default -> throw new IllegalArgumentException(register + " is not an integer data register");
    }
  }

  private void saveFloat(Register register, float value, boolean bulkUpdate) {
    switch (register) {
      case X_POSITION -> {
        Float validated =
            validatePosition(
                Register.X_POSITION,
                m_xPositionMillimeters,
                value,
                POSITION_CHANGE_LIMIT_MM,
                m_haveXPosition,
                bulkUpdate);
        if (validated != null) {
          m_xPositionMillimeters = validated;
          m_haveXPosition = true;
        }
      }
      case Y_POSITION -> {
        Float validated =
            validatePosition(
                Register.Y_POSITION,
                m_yPositionMillimeters,
                value,
                POSITION_CHANGE_LIMIT_MM,
                m_haveYPosition,
                bulkUpdate);
        if (validated != null) {
          m_yPositionMillimeters = validated;
          m_haveYPosition = true;
        }
      }
      case H_ORIENTATION -> {
        Float validated =
            validatePosition(
                Register.H_ORIENTATION,
                m_headingRadians,
                value,
                HEADING_CHANGE_LIMIT_RADIANS,
                m_haveHeading,
                bulkUpdate);
        if (validated != null) {
          m_headingRadians = validated;
          m_haveHeading = true;
        }
      }
      case X_VELOCITY -> {
        Float validated =
            validateVelocity(Register.X_VELOCITY, value, VELOCITY_LIMIT_MM_PER_SECOND, bulkUpdate);
        if (validated != null) {
          m_xVelocityMillimetersPerSecond = validated;
        }
      }
      case Y_VELOCITY -> {
        Float validated =
            validateVelocity(Register.Y_VELOCITY, value, VELOCITY_LIMIT_MM_PER_SECOND, bulkUpdate);
        if (validated != null) {
          m_yVelocityMillimetersPerSecond = validated;
        }
      }
      case H_VELOCITY -> {
        Float validated =
            validateVelocity(
                Register.H_VELOCITY, value, HEADING_VELOCITY_LIMIT_RADIANS_PER_SECOND, bulkUpdate);
        if (validated != null) {
          m_headingVelocityRadiansPerSecond = validated;
        }
      }
      case MM_PER_TICK -> m_millimetersPerTick = value;
      case X_POD_OFFSET -> m_xPodOffsetMillimeters = value;
      case Y_POD_OFFSET -> m_yPodOffsetMillimeters = value;
      case YAW_SCALAR -> m_yawScalar = value;
      case QUATERNION_W -> m_quaternionW = value;
      case QUATERNION_X -> m_quaternionX = value;
      case QUATERNION_Y -> m_quaternionY = value;
      case QUATERNION_Z -> m_quaternionZ = value;
      case PITCH -> m_pitchRadians = value;
      case ROLL -> m_rollRadians = value;
      default -> throw new IllegalArgumentException(register + " is not a float data register");
    }
  }

  private Float validatePosition(
      Register register,
      float oldValue,
      float newValue,
      float changeLimit,
      boolean haveOldValue,
      boolean bulkUpdate) {
    if (m_errorDetectionType != ErrorDetectionType.LOCAL_TEST) {
      return newValue;
    }
    boolean missingBulkData = bulkUpdate && m_loopTimeMicroseconds <= 0;
    if (missingBulkData) {
      recordFailure(register, FailureReason.INVALID_LOOP_TIME);
      return null;
    }
    if (!Float.isFinite(newValue)) {
      recordFailure(register, FailureReason.NONFINITE_VALUE);
      return null;
    }
    if (haveOldValue && Math.abs(newValue - oldValue) > changeLimit) {
      recordFailure(register, FailureReason.CHANGE_TOO_LARGE);
      return null;
    }
    return newValue;
  }

  private Float validateVelocity(
      Register register, float newValue, float magnitudeLimit, boolean bulkUpdate) {
    if (m_errorDetectionType != ErrorDetectionType.LOCAL_TEST) {
      return newValue;
    }
    boolean missingBulkData = bulkUpdate && m_loopTimeMicroseconds <= 0;
    if (missingBulkData) {
      recordFailure(register, FailureReason.INVALID_LOOP_TIME);
      return null;
    }
    if (!Float.isFinite(newValue)) {
      recordFailure(register, FailureReason.NONFINITE_VALUE);
      return null;
    }
    if (Math.abs(newValue) > magnitudeLimit) {
      recordFailure(register, FailureReason.VELOCITY_TOO_LARGE);
      return null;
    }
    return newValue;
  }

  private void recordFailure(Register register, FailureReason reason) {
    m_badReadDetected = true;
    m_deviceStatus = DeviceStatus.FAULT_BAD_READ.m_mask;
    m_lastFailedRegister = register;
    m_lastFailureReason = reason;
    m_failureCount++;
    m_failureCounts[register.ordinal()]++;
  }

  private void finishRead(
      int previousStatus, boolean deviceStatusRead, boolean clearPreviousBadRead) {
    if (m_badReadDetected) {
      m_deviceStatus = DeviceStatus.FAULT_BAD_READ.m_mask;
    } else if (clearPreviousBadRead
        && !deviceStatusRead
        && decodeStatus(previousStatus) == DeviceStatus.FAULT_BAD_READ) {
      m_deviceStatus = DeviceStatus.READY.m_mask;
    }
  }

  private static DeviceStatus decodeStatus(int status) {
    if ((status & DeviceStatus.CALIBRATING.m_mask) != 0) {
      return DeviceStatus.CALIBRATING;
    }
    boolean xPodDetected = (status & DeviceStatus.FAULT_X_POD_NOT_DETECTED.m_mask) == 0;
    boolean yPodDetected = (status & DeviceStatus.FAULT_Y_POD_NOT_DETECTED.m_mask) == 0;
    if (!xPodDetected && !yPodDetected) {
      return DeviceStatus.FAULT_NO_PODS_DETECTED;
    }
    if (!xPodDetected) {
      return DeviceStatus.FAULT_X_POD_NOT_DETECTED;
    }
    if (!yPodDetected) {
      return DeviceStatus.FAULT_Y_POD_NOT_DETECTED;
    }
    if ((status & DeviceStatus.FAULT_IMU_RUNAWAY.m_mask) != 0) {
      return DeviceStatus.FAULT_IMU_RUNAWAY;
    }
    if ((status & DeviceStatus.FAULT_BAD_READ.m_mask) != 0) {
      return DeviceStatus.FAULT_BAD_READ;
    }
    if ((status & DeviceStatus.READY.m_mask) != 0) {
      return DeviceStatus.READY;
    }
    return DeviceStatus.NOT_READY;
  }

  private static byte[] encodeInt(int value) {
    return ByteBuffer.allocate(REGISTER_LENGTH)
        .order(ByteOrder.LITTLE_ENDIAN)
        .putInt(value)
        .array();
  }

  private static byte[] encodeFloat(float value) {
    return ByteBuffer.allocate(REGISTER_LENGTH)
        .order(ByteOrder.LITTLE_ENDIAN)
        .putFloat(value)
        .array();
  }

  private static int decodeInt(byte[] data, int offset) {
    return ByteBuffer.wrap(data, offset, REGISTER_LENGTH).order(ByteOrder.LITTLE_ENDIAN).getInt();
  }

  private static float decodeFloat(byte[] data, int offset) {
    return ByteBuffer.wrap(data, offset, REGISTER_LENGTH).order(ByteOrder.LITTLE_ENDIAN).getFloat();
  }

  private static boolean checkCrc(byte[] data, int dataLength) {
    return data.length > dataLength && computeCrc8(data, dataLength) == data[dataLength];
  }

  private static byte computeCrc8(byte[] data, int length) {
    byte crc = CRC_INITIAL_VALUE;
    for (int index = 0; index < length; index++) {
      crc ^= data[index];
      for (int bit = 0; bit < 8; bit++) {
        if ((crc & 0x80) != 0) {
          crc = (byte) ((crc << 1) ^ CRC_POLYNOMIAL_VALUE);
        } else {
          crc <<= 1;
        }
      }
    }
    return crc;
  }
}
