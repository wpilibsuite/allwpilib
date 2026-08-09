// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drivers.motor;

import java.util.Objects;
import java.util.concurrent.atomic.AtomicBoolean;
import org.wpilib.hardware.bus.CANBusMap;
import org.wpilib.hardware.hal.A301FirmwareVersion;
import org.wpilib.hardware.hal.A301JNI;
import org.wpilib.hardware.hal.A301PeriodicStatus0;
import org.wpilib.hardware.hal.A301PeriodicStatus1;
import org.wpilib.hardware.hal.A301PeriodicStatus2;
import org.wpilib.hardware.hal.A301PeriodicStatus3;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.motor.MotorController;

/** High-level interface to a FIRST A301 motor controller. */
public class A301 implements MotorController, AutoCloseable {
  /** The factory-default A301 device ID. */
  public static final int kDefaultDeviceId = A301JNI.DEFAULT_DEVICE_ID;

  /** A periodic status frame produced by the A301. */
  public enum PeriodicFrame {
    /** Applied output, voltage, current, temperature, inversion, and gearbox information. */
    STATUS_0(A301JNI.STATUS_0),
    /** Fault and warning information. */
    STATUS_1(A301JNI.STATUS_1),
    /** Relative encoder position and velocity. */
    STATUS_2(A301JNI.STATUS_2),
    /** Absolute encoder position. */
    STATUS_3(A301JNI.STATUS_3);

    private final int m_value;

    PeriodicFrame(int value) {
      m_value = value;
    }

    /**
     * Returns the frame corresponding to a HAL frame ID.
     *
     * @param id HAL frame ID
     * @return the periodic frame
     * @throws IllegalArgumentException if the ID is unknown
     */
    public static PeriodicFrame fromId(int id) {
      return switch (id) {
        case A301JNI.STATUS_0 -> STATUS_0;
        case A301JNI.STATUS_1 -> STATUS_1;
        case A301JNI.STATUS_2 -> STATUS_2;
        case A301JNI.STATUS_3 -> STATUS_3;
        default -> throw new IllegalArgumentException("Unknown A301 periodic frame ID: " + id);
      };
    }
  }

  /** A301 gearbox speed variant. */
  public enum GearboxRPM {
    /** The gearbox speed is unknown. */
    UNKNOWN,
    /** 215 RPM gearbox. */
    RPM_215,
    /** 500 RPM gearbox. */
    RPM_500;

    /**
     * Returns the gearbox variant corresponding to a HAL value.
     *
     * @param id HAL gearbox value
     * @return the gearbox variant
     * @throws IllegalArgumentException if the value is unknown
     */
    public static GearboxRPM fromId(int id) {
      return switch (id) {
        case A301JNI.GEARBOX_RPM_UNKNOWN -> UNKNOWN;
        case A301JNI.GEARBOX_RPM_215 -> RPM_215;
        case A301JNI.GEARBOX_RPM_500 -> RPM_500;
        default -> throw new IllegalArgumentException("Unknown A301 gearbox RPM ID: " + id);
      };
    }
  }

  /** Neutral behavior when the controller output is zero. */
  public enum IdleMode {
    /** Allow the motor to coast. */
    COAST(A301JNI.IDLE_MODE_COAST),
    /** Actively brake the motor. */
    BRAKE(A301JNI.IDLE_MODE_BRAKE);

    private final int m_value;

    IdleMode(int value) {
      m_value = value;
    }

    private static IdleMode fromId(int id) {
      return switch (id) {
        case A301JNI.IDLE_MODE_COAST -> COAST;
        case A301JNI.IDLE_MODE_BRAKE -> BRAKE;
        default -> throw new IllegalArgumentException("Unknown A301 idle mode ID: " + id);
      };
    }
  }

  /** Active or sticky A301 motor faults. */
  public static final class Faults {
    public final boolean other;
    public final boolean motorType;
    public final boolean sensor;
    public final boolean can;
    public final boolean temperature;
    public final boolean gateDriver;
    public final boolean escEeprom;
    public final boolean firmware;
    public final boolean motorStartup;
    public final int rawBits;

    private Faults(int rawBits) {
      this.rawBits = rawBits;
      other = getBit(rawBits, 0);
      motorType = getBit(rawBits, 1);
      sensor = getBit(rawBits, 2);
      can = getBit(rawBits, 3);
      temperature = getBit(rawBits, 4);
      gateDriver = getBit(rawBits, 5);
      escEeprom = getBit(rawBits, 6);
      firmware = getBit(rawBits, 7);
      motorStartup = getBit(rawBits, 8);
    }
  }

  /** Active or sticky A301 motor warnings. */
  public static final class Warnings {
    public final boolean brownout;
    public final boolean overcurrent;
    public final boolean escEeprom;
    public final boolean extEeprom;
    public final boolean sensor;
    public final boolean stall;
    public final boolean hasReset;
    public final boolean other;
    public final boolean overvoltage;
    public final boolean motorLoopSpeed;
    public final int rawBits;

    private Warnings(int rawBits) {
      this.rawBits = rawBits;
      brownout = getBit(rawBits, 0);
      overcurrent = getBit(rawBits, 1);
      escEeprom = getBit(rawBits, 2);
      extEeprom = getBit(rawBits, 3);
      sensor = getBit(rawBits, 4);
      stall = getBit(rawBits, 5);
      hasReset = getBit(rawBits, 6);
      other = getBit(rawBits, 7);
      overvoltage = getBit(rawBits, 8);
      motorLoopSpeed = getBit(rawBits, 9);
    }
  }

  private enum ControlType {
    DUTY_CYCLE(A301JNI.CONTROL_TYPE_DUTY_CYCLE),
    VELOCITY(A301JNI.CONTROL_TYPE_VELOCITY),
    VOLTAGE(A301JNI.CONTROL_TYPE_VOLTAGE),
    RELATIVE_POSITION(A301JNI.CONTROL_TYPE_RELATIVE_POSITION),
    ABSOLUTE_POSITION(A301JNI.CONTROL_TYPE_ABSOLUTE_POSITION),
    CURRENT(A301JNI.CONTROL_TYPE_CURRENT);

    private final int m_value;

    ControlType(int value) {
      m_value = value;
    }
  }

  private static final double kDefaultPositionSpeed = 0.0;

  private final int m_handle;
  private final int m_busId;
  private final int m_deviceId;
  private final AtomicBoolean m_isClosed = new AtomicBoolean();
  private volatile double m_setpoint;
  private volatile String m_firmwareString;

  /**
   * Constructs an A301 using its factory-default device ID.
   *
   * <p>On a Motioncore CAN bus, the connected A301 device ID is automatically detected when
   * possible.
   *
   * @param bus the CAN bus
   */
  public A301(CANBusMap bus) {
    this(bus, kDefaultDeviceId);
  }

  /**
   * Constructs an A301.
   *
   * <p>On a Motioncore CAN bus, the connected A301 device ID is automatically detected when
   * possible and the supplied device ID is used as a fallback.
   *
   * @param bus the CAN bus
   * @param deviceId the requested device ID
   */
  public A301(CANBusMap bus, int deviceId) {
    m_handle = A301JNI.initialize(Objects.requireNonNull(bus, "bus").value, deviceId);
    m_busId = A301JNI.getBusId(m_handle);
    m_deviceId = A301JNI.getDeviceId(m_handle);
    HAL.reportUsage("A301[" + m_busId + "][" + m_deviceId + "]", "");
  }

  /** Releases the native A301 handle. */
  @Override
  public void close() {
    if (m_isClosed.compareAndSet(false, true)) {
      A301JNI.free(m_handle);
    }
  }

  /**
   * Returns the CAN bus ID.
   *
   * @return CAN bus ID
   */
  public int getBusId() {
    throwIfClosed();
    return m_busId;
  }

  /**
   * Returns the resolved device ID. This may differ from the requested ID after automatic detection
   * on a Motioncore bus.
   *
   * @return resolved device ID
   */
  public int getDeviceId() {
    throwIfClosed();
    return m_deviceId;
  }

  /**
   * Returns the raw firmware version as {@code major.minor.patch} packed into an integer.
   *
   * @return raw firmware version
   */
  public int getFirmwareVersion() {
    throwIfClosed();
    return A301JNI.getFirmwareVersion(m_handle).raw;
  }

  /**
   * Returns the firmware version as a human-readable string.
   *
   * @return firmware version in {@code vMajor.Minor.Patch} form
   */
  public String getFirmwareString() {
    throwIfClosed();
    String result = m_firmwareString;
    if (result == null) {
      A301FirmwareVersion version = A301JNI.getFirmwareVersion(m_handle);
      result = "v" + version.major + "." + version.minor + "." + version.patch;
      m_firmwareString = result;
    }
    return result;
  }

  /**
   * Returns whether one or more active faults are present.
   *
   * @return active-fault signal
   */
  public A301StatusSignal<Boolean> hasActiveFault() {
    return getFaults().map(faults -> faults.rawBits != 0);
  }

  /**
   * Returns whether one or more sticky faults are present.
   *
   * @return sticky-fault signal
   */
  public A301StatusSignal<Boolean> hasStickyFault() {
    return getStickyFaults().map(faults -> faults.rawBits != 0);
  }

  /**
   * Returns whether one or more active warnings are present.
   *
   * @return active-warning signal
   */
  public A301StatusSignal<Boolean> hasActiveWarning() {
    return getWarnings().map(warnings -> warnings.rawBits != 0);
  }

  /**
   * Returns whether one or more sticky warnings are present.
   *
   * @return sticky-warning signal
   */
  public A301StatusSignal<Boolean> hasStickyWarning() {
    return getStickyWarnings().map(warnings -> warnings.rawBits != 0);
  }

  /**
   * Returns active motor faults.
   *
   * @return active-fault signal
   */
  public A301StatusSignal<Faults> getFaults() {
    return getPeriodicStatus1().map(frame -> new Faults(frame.faults));
  }

  /**
   * Returns sticky motor faults recorded since they were last cleared.
   *
   * @return sticky-fault signal
   */
  public A301StatusSignal<Faults> getStickyFaults() {
    return getPeriodicStatus1().map(frame -> new Faults(frame.stickyFaults));
  }

  /**
   * Returns active motor warnings.
   *
   * @return active-warning signal
   */
  public A301StatusSignal<Warnings> getWarnings() {
    return getPeriodicStatus1().map(frame -> new Warnings(frame.warnings));
  }

  /**
   * Returns sticky motor warnings recorded since they were last cleared.
   *
   * @return sticky-warning signal
   */
  public A301StatusSignal<Warnings> getStickyWarnings() {
    return getPeriodicStatus1().map(frame -> new Warnings(frame.stickyWarnings));
  }

  /**
   * Clears active and sticky faults and warnings.
   *
   * @return command status
   */
  public A301Error clearFaults() {
    throwIfClosed();
    return A301Error.fromHalStatus(A301JNI.clearFaults(m_handle));
  }

  /**
   * Returns the controller input voltage in volts.
   *
   * @return bus-voltage signal
   */
  public A301StatusSignal<Double> getBusVoltage() {
    return getPeriodicStatus0().map(frame -> frame.voltage);
  }

  /**
   * Returns the applied output duty cycle.
   *
   * @return applied-output signal
   */
  public A301StatusSignal<Double> getAppliedOutput() {
    return getPeriodicStatus0().map(frame -> frame.appliedOutput);
  }

  /**
   * Returns motor current in amperes.
   *
   * @return motor-current signal
   */
  public A301StatusSignal<Double> getMotorCurrent() {
    return getPeriodicStatus0().map(frame -> frame.current);
  }

  /**
   * Returns motor temperature in degrees Celsius.
   *
   * @return motor-temperature signal
   */
  public A301StatusSignal<Double> getMotorTemperature() {
    return getPeriodicStatus0().map(frame -> (double) frame.motorTemperature);
  }

  /**
   * Returns the attached gearbox speed variant.
   *
   * @return gearbox signal
   */
  public A301StatusSignal<GearboxRPM> getGearboxRPM() {
    return getPeriodicStatus0().map(frame -> GearboxRPM.fromId(frame.gearboxRPM));
  }

  /**
   * Returns relative encoder position in motor rotations.
   *
   * @return relative-position signal
   */
  public A301StatusSignal<Double> getRelativeEncoderPosition() {
    return getPeriodicStatus2().map(frame -> frame.relativeEncoderPosition);
  }

  /**
   * Returns relative encoder velocity in RPM.
   *
   * @return encoder-velocity signal
   */
  public A301StatusSignal<Double> getEncoderVelocity() {
    return getPeriodicStatus2().map(frame -> frame.encoderVelocity);
  }

  /**
   * Returns absolute encoder position in rotations.
   *
   * @return absolute-position signal
   */
  public A301StatusSignal<Double> getAbsoluteEncoderPosition() {
    return getPeriodicStatus3().map(frame -> frame.absoluteEncoderPosition);
  }

  /**
   * Sets relative encoder position in motor rotations.
   *
   * @param position position in rotations
   * @return command status
   */
  public A301Error setRelativeEncoderPosition(double position) {
    throwIfClosed();
    return A301Error.fromHalStatus(A301JNI.setRelativeEncoderPosition(m_handle, position));
  }

  /**
   * Sets absolute encoder position in rotations.
   *
   * @param position position in rotations
   * @return command status
   */
  public A301Error setAbsoluteEncoderPosition(double position) {
    throwIfClosed();
    return A301Error.fromHalStatus(A301JNI.setAbsoluteEncoderPosition(m_handle, position));
  }

  /**
   * Sets a velocity setpoint in RPM.
   *
   * @param velocity velocity in RPM
   * @return command status
   */
  public A301Error setVelocity(double velocity) {
    return setSetpoint(ControlType.VELOCITY, velocity);
  }

  /**
   * Sets a relative position setpoint using maximum speed.
   *
   * @param position position in motor rotations
   * @return command status
   */
  public A301Error setRelativePosition(double position) {
    return setSetpoint(ControlType.RELATIVE_POSITION, position);
  }

  /**
   * Sets a relative position setpoint with a specific approach speed.
   *
   * <p>A speed less than or equal to zero requests maximum speed.
   *
   * @param position position in motor rotations
   * @param speed approach speed in RPM
   * @return command status
   */
  public A301Error setRelativePositionWithSpeed(double position, double speed) {
    return setSetpoint(ControlType.RELATIVE_POSITION, position, speed);
  }

  /**
   * Sets an absolute position setpoint using maximum speed.
   *
   * @param position absolute position in rotations
   * @return command status
   */
  public A301Error setAbsolutePosition(double position) {
    return setSetpoint(ControlType.ABSOLUTE_POSITION, position);
  }

  /**
   * Sets an absolute position setpoint with a specific approach speed.
   *
   * <p>A speed less than or equal to zero requests maximum speed.
   *
   * @param position absolute position in rotations
   * @param speed approach speed in RPM
   * @return command status
   */
  public A301Error setAbsolutePositionWithSpeed(double position, double speed) {
    return setSetpoint(ControlType.ABSOLUTE_POSITION, position, speed);
  }

  /**
   * Sets a motor-current setpoint.
   *
   * @param current current in amperes
   * @return command status
   */
  public A301Error setCurrent(double current) {
    return setSetpoint(ControlType.CURRENT, current);
  }

  /**
   * Sets the idle mode.
   *
   * @param idleMode idle mode
   * @return command status
   */
  public A301Error setIdleMode(IdleMode idleMode) {
    throwIfClosed();
    return A301Error.fromHalStatus(
        A301JNI.setIdleMode(m_handle, Objects.requireNonNull(idleMode, "idleMode").m_value));
  }

  /**
   * Returns the configured idle mode.
   *
   * @return idle mode
   */
  public IdleMode getIdleMode() {
    throwIfClosed();
    return IdleMode.fromId(A301JNI.getIdleMode(m_handle));
  }

  /**
   * Enables continuous input for absolute position control.
   *
   * @return command status
   */
  public A301Error enableAbsolutePositionContinuousInput() {
    return setAbsolutePositionContinuousInput(true);
  }

  /**
   * Disables continuous input for absolute position control.
   *
   * @return command status
   */
  public A301Error disableAbsolutePositionContinuousInput() {
    return setAbsolutePositionContinuousInput(false);
  }

  /**
   * Returns whether continuous input is enabled for absolute position control.
   *
   * @return true when continuous input is enabled
   */
  public boolean isAbsolutePositionContinuousInputEnabled() {
    throwIfClosed();
    return A301JNI.getAbsolutePositionContinuousInput(m_handle);
  }

  /**
   * Sets the absolute encoder range offset.
   *
   * @param offset offset in rotations, from -0.5 to 0.5
   * @return command status
   */
  public A301Error setAbsoluteEncoderRangeOffset(double offset) {
    throwIfClosed();
    return A301Error.fromHalStatus(A301JNI.setAbsoluteEncoderRangeOffset(m_handle, offset));
  }

  /**
   * Returns the absolute encoder range offset.
   *
   * @return range offset in rotations
   */
  public double getAbsoluteEncoderRangeOffset() {
    throwIfClosed();
    return A301JNI.getAbsoluteEncoderRangeOffset(m_handle);
  }

  /**
   * Sets the controller duty cycle.
   *
   * @param throttle duty cycle, conventionally from -1.0 to 1.0
   */
  @Override
  public void setThrottle(double throttle) {
    throwIfClosed();
    m_setpoint = throttle;
    setSetpoint(ControlType.DUTY_CYCLE, throttle);
  }

  /**
   * Returns the last commanded duty cycle.
   *
   * @return last commanded duty cycle
   */
  @Override
  public double getThrottle() {
    throwIfClosed();
    return m_setpoint;
  }

  /**
   * Sets the controller's internal voltage setpoint.
   *
   * <p>Unlike the default {@link MotorController} implementation, the A301 regulates this voltage
   * internally, so this is a set-and-forget command.
   *
   * @param outputVolts voltage setpoint in volts
   */
  @Override
  public void setVoltage(double outputVolts) {
    throwIfClosed();
    m_setpoint = outputVolts / 12.0;
    setSetpoint(ControlType.VOLTAGE, outputVolts);
  }

  /**
   * Sets whether non-position control output is inverted.
   *
   * @param isInverted true to invert the output
   */
  @Override
  public void setInverted(boolean isInverted) {
    throwIfClosed();
    A301JNI.setInverted(m_handle, isInverted);
  }

  /**
   * Returns whether non-position control output is inverted.
   *
   * @return true when the output is inverted
   */
  @Override
  public boolean getInverted() {
    throwIfClosed();
    return A301JNI.getInverted(m_handle);
  }

  /** Stops the motor output. */
  @Override
  public void disable() {
    throwIfClosed();
    m_setpoint = 0.0;
    setSetpoint(ControlType.DUTY_CYCLE, 0.0);
  }

  /**
   * Sets a periodic status frame period.
   *
   * <p>The controller may normalize the requested period. Use {@link #getStatusFramePeriod} to
   * retrieve the effective value.
   *
   * @param frame periodic status frame
   * @param periodMs requested period in milliseconds, from 0 to 1000
   * @return this object for method chaining
   */
  public A301 setStatusFramePeriod(PeriodicFrame frame, int periodMs) {
    throwIfClosed();
    A301JNI.setStatusFramePeriod(
        m_handle, Objects.requireNonNull(frame, "frame").m_value, periodMs);
    return this;
  }

  /**
   * Returns the effective periodic status frame period.
   *
   * @param frame periodic status frame
   * @return period in milliseconds
   */
  public int getStatusFramePeriod(PeriodicFrame frame) {
    throwIfClosed();
    return A301JNI.getStatusFramePeriod(m_handle, Objects.requireNonNull(frame, "frame").m_value);
  }

  /**
   * Sets the fault signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 faultsPeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_1, periodMs);
  }

  /**
   * Returns the effective fault signal period.
   *
   * @return period in milliseconds
   */
  public int getFaultsPeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_1);
  }

  /**
   * Sets the warning signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 warningsPeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_1, periodMs);
  }

  /**
   * Returns the effective warning signal period.
   *
   * @return period in milliseconds
   */
  public int getWarningsPeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_1);
  }

  /**
   * Sets the bus-voltage signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 busVoltagePeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_0, periodMs);
  }

  /**
   * Returns the effective bus-voltage signal period.
   *
   * @return period in milliseconds
   */
  public int getBusVoltagePeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_0);
  }

  /**
   * Sets the applied-output signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 appliedOutputPeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_0, periodMs);
  }

  /**
   * Returns the effective applied-output signal period.
   *
   * @return period in milliseconds
   */
  public int getAppliedOutputPeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_0);
  }

  /**
   * Sets the motor-current signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 motorCurrentPeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_0, periodMs);
  }

  /**
   * Returns the effective motor-current signal period.
   *
   * @return period in milliseconds
   */
  public int getMotorCurrentPeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_0);
  }

  /**
   * Sets the motor-temperature signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 motorTemperaturePeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_0, periodMs);
  }

  /**
   * Returns the effective motor-temperature signal period.
   *
   * @return period in milliseconds
   */
  public int getMotorTemperaturePeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_0);
  }

  /**
   * Sets the relative-encoder-position signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 relativeEncoderPositionPeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_2, periodMs);
  }

  /**
   * Returns the effective relative-encoder-position signal period.
   *
   * @return period in milliseconds
   */
  public int getRelativeEncoderPositionPeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_2);
  }

  /**
   * Sets the encoder-velocity signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 encoderVelocityPeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_2, periodMs);
  }

  /**
   * Returns the effective encoder-velocity signal period.
   *
   * @return period in milliseconds
   */
  public int getEncoderVelocityPeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_2);
  }

  /**
   * Sets the absolute-encoder-position signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  public A301 absoluteEncoderPositionPeriodMs(int periodMs) {
    return setStatusFramePeriod(PeriodicFrame.STATUS_3, periodMs);
  }

  /**
   * Returns the effective absolute-encoder-position signal period.
   *
   * @return period in milliseconds
   */
  public int getAbsoluteEncoderPositionPeriodMs() {
    return getStatusFramePeriod(PeriodicFrame.STATUS_3);
  }

  private static boolean getBit(int value, int bit) {
    return (value & (1 << bit)) != 0;
  }

  private A301Error setAbsolutePositionContinuousInput(boolean enabled) {
    throwIfClosed();
    return A301Error.fromHalStatus(A301JNI.setAbsolutePositionContinuousInput(m_handle, enabled));
  }

  private A301StatusSignal<A301PeriodicStatus0> getPeriodicStatus0() {
    throwIfClosed();
    A301PeriodicStatus0 frame = A301JNI.getPeriodicStatus0(m_handle);
    return A301StatusSignal.of(frame, frame.status, frame.timestamp);
  }

  private A301StatusSignal<A301PeriodicStatus1> getPeriodicStatus1() {
    throwIfClosed();
    A301PeriodicStatus1 frame = A301JNI.getPeriodicStatus1(m_handle);
    return A301StatusSignal.of(frame, frame.status, frame.timestamp);
  }

  private A301StatusSignal<A301PeriodicStatus2> getPeriodicStatus2() {
    throwIfClosed();
    A301PeriodicStatus2 frame = A301JNI.getPeriodicStatus2(m_handle);
    return A301StatusSignal.of(frame, frame.status, frame.timestamp);
  }

  private A301StatusSignal<A301PeriodicStatus3> getPeriodicStatus3() {
    throwIfClosed();
    A301PeriodicStatus3 frame = A301JNI.getPeriodicStatus3(m_handle);
    return A301StatusSignal.of(frame, frame.status, frame.timestamp);
  }

  private A301Error setSetpoint(ControlType controlType, double setpoint) {
    return setSetpoint(controlType, setpoint, kDefaultPositionSpeed);
  }

  private A301Error setSetpoint(ControlType controlType, double setpoint, double positionSpeed) {
    throwIfClosed();
    return A301Error.fromHalStatus(
        A301JNI.setSetpoint(m_handle, setpoint, controlType.m_value, positionSpeed));
  }

  private void throwIfClosed() {
    if (m_isClosed.get()) {
      throw new IllegalStateException("This FIRST A301 object has been closed");
    }
  }
}
