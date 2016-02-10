/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.CanTalonJNI;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;

public class CANTalon implements MotorSafety, PIDOutput, PIDSource, CANSpeedController {
  private MotorSafetyHelper m_safetyHelper;
  private boolean isInverted = false;
  protected PIDSourceType m_pidSource = PIDSourceType.kDisplacement;
  /**
   * Number of adc engineering units per 0 to 3.3V sweep.
   * This is necessary for scaling Analog Position in rotations/RPM.
   */
  private final int kNativeAdcUnitsPerRotation = 1024;
  /**
   * Number of pulse width engineering units per full rotation.
   * This is necessary for scaling Pulse Width Decoded Position in rotations/RPM.
   */
  private final double kNativePwdUnitsPerRotation = 4096.0;
  /**
   * Number of minutes per 100ms unit.  Useful for scaling velocities
   * measured by Talon's 100ms timebase to rotations per minute.
   */
  private final double kMinutesPer100msUnit = 1.0/600.0;

  public enum TalonControlMode implements CANSpeedController.ControlMode {
    PercentVbus(0), Position(1), Speed(2), Current(3), Voltage(4), Follower(5), MotionProfile(6), Disabled(15);

    public final int value;

    public static TalonControlMode valueOf(int value) {
      for (TalonControlMode mode : values()) {
        if (mode.value == value) {
          return mode;
        }
      }

      return null;
    }

    private TalonControlMode(int value) {
      this.value = value;
    }

    @Override
    public boolean isPID() {
        return this == Current || this == Speed || this == Position;
    }

    @Override
    public int getValue() {
        return value;
    }
  }
  public enum FeedbackDevice {
    QuadEncoder(0), AnalogPot(2), AnalogEncoder(3), EncRising(4), EncFalling(5), CtreMagEncoder_Relative(6), CtreMagEncoder_Absolute(7), PulseWidth(8);

    public int value;

    public static FeedbackDevice valueOf(int value) {
      for (FeedbackDevice mode : values()) {
        if (mode.value == value) {
          return mode;
        }
      }

      return null;
    }

    private FeedbackDevice(int value) {
      this.value = value;
    }
  }
  /**
   * Depending on the sensor type, Talon can determine if sensor is plugged in ot not.
   */
  public enum FeedbackDeviceStatus {
    FeedbackStatusUnknown(0), FeedbackStatusPresent(1), FeedbackStatusNotPresent(2);
    public int value;
    public static FeedbackDeviceStatus valueOf(int value) {
      for (FeedbackDeviceStatus mode : values()) {
        if (mode.value == value) {
          return mode;
        }
      }
      return null;
    }
    private FeedbackDeviceStatus(int value) {
      this.value = value;
    }
  }
  /** enumerated types for frame rate ms */
  public enum StatusFrameRate {
    General(0), Feedback(1), QuadEncoder(2), AnalogTempVbat(3), PulseWidth(4);
    public int value;

    public static StatusFrameRate valueOf(int value) {
      for (StatusFrameRate mode : values()) {
        if (mode.value == value) {
          return mode;
        }
      }
      return null;
    }

    private StatusFrameRate(int value) {
      this.value = value;
    }
  }
  /**
   * Enumerated types for Motion Control Set Values.
   * When in Motion Profile control mode, these constants are paseed
   * into set() to manipulate the motion profile executer.
   * When changing modes, be sure to read the value back using getMotionProfileStatus()
   * to ensure changes in output take effect before performing buffering actions.
   * Disable will signal Talon to put motor output into neutral drive.
   *   Talon will stop processing motion profile points.  This means the buffer is
   *   effectively disconnected from the executer, allowing the robot to gracefully
   *   clear and push new traj points.  isUnderrun will get cleared.
   *   The active trajectory is also cleared.
   * Enable will signal Talon to pop a trajectory point from it's buffer and process it.
   *   If the active trajectory is empty, Talon will shift in the next point.
   *   If the active traj is empty, and so is the buffer, the motor drive is neutral and
   *   isUnderrun is set.  When active traj times out, and buffer has at least one point,
   *   Talon shifts in next one, and isUnderrun is cleared.  When active traj times out,
   *   and buffer is empty, Talon keeps processing active traj and sets IsUnderrun.
   * Hold will signal Talon keep processing the active trajectory indefinitely.
   *   If the active traj is cleared, Talon will neutral motor drive.  Otherwise
   *    Talon will keep processing the active traj but it will not shift in
   *    points from the buffer.  This means the buffer is  effectively disconnected
   *    from the executer, allowing the robot to gracefully clear and push
   *    new traj points.
   *    isUnderrun is set if active traj is empty, otherwise it is cleared.
   *    isLast signal is also cleared.
   *
   * Typical workflow:
   *   set(Disable),
   *   Confirm Disable takes effect,
   *   clear buffer and push buffer points,
   *   set(Enable) when enough points have been pushed to ensure no underruns,
   *   wait for MP to finish or decide abort,
   *   If MP finished gracefully set(Hold) to hold position servo and disconnect buffer,
   *   If MP is being aborted set(Disable) to neutral the motor and disconnect buffer,
   *   Confirm mode takes effect,
   *   clear buffer and push buffer points, and rinse-repeat.
   */
  public enum SetValueMotionProfile {
    Disable(0), Enable(1), Hold(2);
    public int value;

    public static SetValueMotionProfile valueOf(int value) {
      for (SetValueMotionProfile mode : values()) {
        if (mode.value == value) {
          return mode;
        }
      }
      return null;
    }

    private SetValueMotionProfile(int value) {
      this.value = value;
    }
  }
  /**
   * Motion Profile Trajectory Point
   * This is simply a data transer object.
   */
  public static class TrajectoryPoint {
    public double position; //!< The position to servo to.
    public double velocity; //!< The velocity to feed-forward.
    /**
     * Time in milliseconds to process this point.
     * Value should be between 1ms and 255ms.  If value is zero
     * then Talon will default to 1ms.  If value exceeds 255ms API will cap it.
     */
    public int timeDurMs;
    /**
     * Which slot to get PIDF gains.
     * PID is used for position servo.
     * F is used as the Kv constant for velocity feed-forward.
     * Typically this is hardcoded to the a particular slot, but you are free
     * gain schedule if need be.
     */
    public int profileSlotSelect;
    /**
     * Set to true to only perform the velocity feed-forward and not perform
     * position servo.  This is useful when learning how the position servo
     * changes the motor response.  The same could be accomplish by clearing the
     * PID gains, however this is synchronous the streaming, and doesn't require restoing
     * gains when finished.
     *
     * Additionaly setting this basically gives you direct control of the motor output
     * since motor output = targetVelocity X Kv, where Kv is our Fgain.
     * This means you can also scheduling straight-throttle curves without relying on
     * a sensor.
     */
    public boolean velocityOnly;
    /**
     * Set to true to signal Talon that this is the final point, so do not
     * attempt to pop another trajectory point from out of the Talon buffer.
     * Instead continue processing this way point.  Typically the velocity
     * member variable should be zero so that the motor doesn't spin indefinitely.
     */
    public boolean isLastPoint;
   /**
     * Set to true to signal Talon to zero the selected sensor.
     * When generating MPs, one simple method is to make the first target position zero,
     * and the final target position the target distance from the current position.
     * Then when you fire the MP, the current position gets set to zero.
     * If this is the intent, you can set zeroPos on the first trajectory point.
     *
     * Otherwise you can leave this false for all points, and offset the positions
     * of all trajectory points so they are correct.
     */
    public boolean zeroPos;
  }
  /**
   * Motion Profile Status
   * This is simply a data transer object.
   */
  public static class MotionProfileStatus {
    /**
     * The available empty slots in the trajectory buffer.
     *
     * The robot API holds a "top buffer" of trajectory points, so your applicaion
     * can dump several points at once.  The API will then stream them into the Talon's
     * low-level buffer, allowing the Talon to act on them.
     */
    public int topBufferRem;
    /**
     * The number of points in the top trajectory buffer.
     */
    public int topBufferCnt;
    /**
     * The number of points in the low level Talon buffer.
     */
    public int btmBufferCnt;
    /**
     * Set if isUnderrun ever gets set.
     * Only is cleared by clearMotionProfileHasUnderrun() to ensure
     * robot logic can react or instrument it.
     * @see clearMotionProfileHasUnderrun()
     */
    public boolean hasUnderrun;
    /**
     * This is set if Talon needs to shift a point from its buffer into
     * the active trajectory point however the buffer is empty. This gets cleared
     * automatically when is resolved.
     */
    public boolean isUnderrun;
    /**
     * True if the active trajectory point has not empty, false otherwise.
     * The members in activePoint are only valid if this signal is set.
     */
    public boolean activePointValid;
    /**
     * The number of points in the low level Talon buffer.
     */
    public TrajectoryPoint activePoint = new TrajectoryPoint();
    /**
     * The current output mode of the motion profile executer (disabled, enabled, or hold).
     * When changing the set() value in MP mode, it's important to check this signal to
     * confirm the change takes effect before interacting with the top buffer.
     */
    public SetValueMotionProfile outputEnable;
  }

  private long m_handle;
  private TalonControlMode m_controlMode;
  private static double kDelayForSolicitedSignals = 0.004;
  private double m_minimumInput;
  private double m_maximumInput;

  int m_deviceNumber;
  boolean m_controlEnabled;
  boolean m_stopped = false;
  int m_profile;

  double m_setPoint;
  /**
   * Encoder CPR, counts per rotations, also called codes per revoluion.
   * Default value of zero means the API behaves as it did during the 2015 season, each position
   * unit is a single pulse and there are four pulses per count (4X).
   * Caller can use configEncoderCodesPerRev to set the quadrature encoder CPR.
   */
  int m_codesPerRev;
  /**
   * Number of turns per rotation.  For example, a 10-turn pot spins ten full rotations from
   * a wiper voltage of zero to 3.3 volts.  Therefore knowing the
   * number of turns a full voltage sweep represents is necessary for calculating rotations
   * and velocity.
   * A default value of zero means the API behaves as it did during the 2015 season, there are 1024
   * position units from zero to 3.3V.
   */
  int m_numPotTurns;
  /**
   * Although the Talon handles feedback selection, caching the feedback selection is helpful at the API level
   * for scaling into rotations and RPM.
   */
  FeedbackDevice m_feedbackDevice;
  /**
   * Constructor for the CANTalon device.
   * @param deviceNumber The CAN ID of the Talon SRX
   */
  public CANTalon(int deviceNumber) {
    m_deviceNumber = deviceNumber;
    m_handle = CanTalonJNI.new_CanTalonSRX(deviceNumber);
    m_safetyHelper = new MotorSafetyHelper(this);
    m_controlEnabled = true;
    m_profile = 0;
    m_setPoint = 0;
    m_codesPerRev = 0;
    m_numPotTurns = 0;
    m_feedbackDevice = FeedbackDevice.QuadEncoder;
    setProfile(m_profile);
    applyControlMode(TalonControlMode.PercentVbus);
    LiveWindow.addActuator("CANTalon", m_deviceNumber, this);
  }
  /**
   * Constructor for the CANTalon device.
   * @param deviceNumber The CAN ID of the Talon SRX
   * @param controlPeriodMs The period in ms to send the CAN control frame.
   *                        Period is bounded to [1ms,95ms].
   */
  public CANTalon(int deviceNumber, int controlPeriodMs) {
    m_deviceNumber = deviceNumber;
    /* bound period to be within [1 ms,95 ms] */
    m_handle = CanTalonJNI.new_CanTalonSRX(deviceNumber, controlPeriodMs);
    m_safetyHelper = new MotorSafetyHelper(this);
    m_controlEnabled = true;
    m_profile = 0;
    m_setPoint = 0;
    m_codesPerRev = 0;
    m_numPotTurns = 0;
    m_feedbackDevice = FeedbackDevice.QuadEncoder;
    setProfile(m_profile);
    applyControlMode(TalonControlMode.PercentVbus);
    LiveWindow.addActuator("CANTalon", m_deviceNumber, this);
  }
  /**
   * Constructor for the CANTalon device.
   * @param deviceNumber The CAN ID of the Talon SRX
   * @param controlPeriodMs The period in ms to send the CAN control frame.
   *                        Period is bounded to [1ms,95ms].
   * @param enablePeriodMs The period in ms to send the enable control frame.
   *                        Period is bounded to [1ms,95ms]. This typically is not
   *                        required to specify, however this could be used to minimize the
   *                        time between robot-enable and talon-motor-drive.
   */
  public CANTalon(int deviceNumber, int controlPeriodMs, int enablePeriodMs) {
    m_deviceNumber = deviceNumber;
    m_handle = CanTalonJNI.new_CanTalonSRX(deviceNumber, controlPeriodMs, enablePeriodMs);
    m_safetyHelper = new MotorSafetyHelper(this);
    m_controlEnabled = true;
    m_profile = 0;
    m_setPoint = 0;
    m_codesPerRev = 0;
    m_numPotTurns = 0;
    m_feedbackDevice = FeedbackDevice.QuadEncoder;
    setProfile(m_profile);
    applyControlMode(TalonControlMode.PercentVbus);
    LiveWindow.addActuator("CANTalon", m_deviceNumber, this);
  }

  @Override
  public void pidWrite(double output) {
    if (getControlMode() == TalonControlMode.PercentVbus) {
      set(output);
    } else {
      throw new IllegalStateException("PID only supported in PercentVbus mode");
    }
  }

  /**
   * {@inheritDoc}
   */
  public void setPIDSourceType(PIDSourceType pidSource) {
    m_pidSource = pidSource;
  }

  /**
   * {@inheritDoc}
   */
  public PIDSourceType getPIDSourceType() {
    return m_pidSource;
  }

  @Override
  public double pidGet() {
    return getPosition();
  }

  public void delete() {
    disable();
    if (m_handle != 0) {
      CanTalonJNI.delete_CanTalonSRX(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Sets the appropriate output on the talon, depending on the mode.
   *
   * In PercentVbus, the output is between -1.0 and 1.0, with 0.0 as stopped. In
   * Follower mode, the output is the integer device ID of the talon to
   * duplicate. In Voltage mode, outputValue is in volts. In Current mode,
   * outputValue is in amperes. In Speed mode, outputValue is in position change
   * / 10ms. In Position mode, outputValue is in encoder ticks or an analog
   * value, depending on the sensor.
   *
   * @param outputValue The setpoint value, as described above.
   */
  public void set(double outputValue) {
    /* feed safety helper since caller just updated our output */
    m_safetyHelper.feed();
    if(m_stopped) {
      enableControl();
      m_stopped = false;
    }
    if (m_controlEnabled) {
      m_setPoint = outputValue; /* cache set point for getSetpoint() */
      switch (m_controlMode) {
        case PercentVbus:
          CanTalonJNI.Set(m_handle, isInverted ? -outputValue : outputValue);
          break;
        case Follower:
          CanTalonJNI.SetDemand(m_handle, (int) outputValue);
          break;
        case Voltage:
          // Voltage is an 8.8 fixed point number.
          int volts = (int) ((isInverted ? -outputValue : outputValue) * 256);
          CanTalonJNI.SetDemand(m_handle, volts);
          break;
        case Speed:
          CanTalonJNI.SetDemand(m_handle, ScaleVelocityToNativeUnits(m_feedbackDevice,(isInverted ? -outputValue : outputValue)));
          break;
        case Position:
          CanTalonJNI.SetDemand(m_handle, ScaleRotationsToNativeUnits(m_feedbackDevice,outputValue));
          break;
        case Current:
          double milliamperes = (isInverted ? -outputValue : outputValue) * 1000.0; /* mA*/
          CanTalonJNI.SetDemand(m_handle, (int)milliamperes);
          break;
        case MotionProfile:
          CanTalonJNI.SetDemand(m_handle, (int) outputValue);
          break;
        default:
          break;
      }
      CanTalonJNI.SetModeSelect(m_handle, m_controlMode.value);
    }
  }

  /**
   * Inverts the direction of the motor's rotation. Only works in PercentVbus
   * mode.
   *
   * @param isInverted The state of inversion, true is inverted.
   */
  @Override
  public void setInverted(boolean isInverted) {
    this.isInverted = isInverted;
  }

  /**
   * Common interface for the inverting direction of a speed controller.
   *
   * @return isInverted The state of inversion, true is inverted.
   *
   */
  @Override
  public boolean getInverted() {
    return this.isInverted;
  }

  /**
   * Sets the output of the Talon.
   *
   * @param outputValue See set().
   * @param thisValueDoesNotDoAnything corresponds to syncGroup from Jaguar; not
   *        relevant here.
   */
  @Override
  public void set(double outputValue, byte thisValueDoesNotDoAnything) {
    set(outputValue);
  }

  /**
   * Resets the accumulated integral error and disables the controller.
   *
   * The only difference between this and {@link PIDController#reset} is that
   * the PIDController also resets the previous error for the D term, but the
   * difference should have minimal effect as it will only last one cycle.
   */
  public void reset() {
    disable();
    clearIAccum();
  }

  /**
   * Return true if Talon is enabled.
   *
   * @return true if the Talon is enabled and may be applying power to the motor
   */
  public boolean isEnabled() {
    return isControlEnabled();
  }

  /**
   * Returns the difference between the setpoint and the current position.
   *
   * @return The error in units corresponding to whichever mode we are in.
   * @see #set(double) set() for a detailed description of the various units.
   */
  public double getError() {
    return getClosedLoopError();
  }

  /**
   * Calls {@link #set(double)}.
   */
  public void setSetpoint(double setpoint) {
    set(setpoint);
  }

  /**
   * Flips the sign (multiplies by negative one) the sensor values going into
   * the talon.
   *
   * This only affects position and velocity closed loop control. Allows for
   * situations where you may have a sensor flipped and going in the wrong
   * direction.
   *
   * @param flip True if sensor input should be flipped; False if not.
   */
  public void reverseSensor(boolean flip) {
    CanTalonJNI.SetRevFeedbackSensor(m_handle, flip ? 1 : 0);
  }

  /**
   * Flips the sign (multiplies by negative one) the throttle values going into
   * the motor on the talon in closed loop modes.
   *
   * @param flip True if motor output should be flipped; False if not.
   */
  public void reverseOutput(boolean flip) {
    CanTalonJNI.SetRevMotDuringCloseLoopEn(m_handle, flip ? 1 : 0);
  }

  /**
   * Gets the current status of the Talon (usually a sensor value).
   *
   * In Current mode: returns output current. In Speed mode: returns current
   * speed. In Position mode: returns current sensor position. In PercentVbus
   * and Follower modes: returns current applied throttle.
   *
   * @return The current sensor value of the Talon.
   */
  public double get() {
    switch (m_controlMode) {
      case Voltage:
        return getOutputVoltage();
      case Current:
        return getOutputCurrent();
      case Speed:
        return ScaleNativeUnitsToRpm(m_feedbackDevice,CanTalonJNI.GetSensorVelocity(m_handle));
      case Position:
        return ScaleNativeUnitsToRotations(m_feedbackDevice,CanTalonJNI.GetSensorPosition(m_handle));
      case PercentVbus:
      default:
        return (double) CanTalonJNI.GetAppliedThrottle(m_handle) / 1023.0;
    }
  }

  /**
   * Get the current encoder position, regardless of whether it is the current
   * feedback device.
   *
   * @return The current position of the encoder.
   */
  public int getEncPosition() {
    return CanTalonJNI.GetEncPosition(m_handle);
  }

  public void setEncPosition(int newPosition) {
    setParameter(CanTalonJNI.param_t.eEncPosition, newPosition);
  }

  /**
   * Get the current encoder velocity, regardless of whether it is the current
   * feedback device.
   *
   * @return The current speed of the encoder.
   */
  public int getEncVelocity() {
    return CanTalonJNI.GetEncVel(m_handle);
  }
  public int getPulseWidthPosition() {
    return CanTalonJNI.GetPulseWidthPosition(m_handle);
  }
  public void setPulseWidthPosition(int newPosition) {
    setParameter(CanTalonJNI.param_t.ePwdPosition, newPosition);
  }
  public int getPulseWidthVelocity() {
    return CanTalonJNI.GetPulseWidthVelocity(m_handle);
  }
  public int getPulseWidthRiseToFallUs() {
    return CanTalonJNI.GetPulseWidthRiseToFallUs(m_handle);
  }
  public int getPulseWidthRiseToRiseUs() {
    return CanTalonJNI.GetPulseWidthRiseToRiseUs(m_handle);
  }
  /**
   * @param feedbackDevice which feedback sensor to check it if is connected.
   * @return status of caller's specified sensor type.
   */
  public FeedbackDeviceStatus isSensorPresent(FeedbackDevice feedbackDevice) {
    FeedbackDeviceStatus retval = FeedbackDeviceStatus.FeedbackStatusUnknown;
    /* detecting sensor health depends on which sensor caller cares about */
    switch(feedbackDevice){
      case QuadEncoder:
      case AnalogPot:
      case AnalogEncoder:
      case EncRising:
      case EncFalling:
        /* no real good way to tell if these sensor
          are actually present so return status unknown. */
        break;
      case PulseWidth:
      case CtreMagEncoder_Relative:
      case CtreMagEncoder_Absolute:
        /* all of these require pulse width signal to be present. */
        if( CanTalonJNI.IsPulseWidthSensorPresent(m_handle) == 0 ){
          /* Talon not getting a signal */
          retval = FeedbackDeviceStatus.FeedbackStatusNotPresent;
        }else{
          /* getting good signal */
          retval = FeedbackDeviceStatus.FeedbackStatusPresent;
        }
        break;
    }
    return retval;
  }
  /**
   * Get the number of of rising edges seen on the index pin.
   *
   * @return number of rising edges on idx pin.
   */
  public int getNumberOfQuadIdxRises() {
    return CanTalonJNI.GetEncIndexRiseEvents(m_handle);
  }

  /**
   * @return IO level of QUADA pin.
   */
  public int getPinStateQuadA() {
    return CanTalonJNI.GetQuadApin(m_handle);
  }

  /**
   * @return IO level of QUADB pin.
   */
  public int getPinStateQuadB() {
    return CanTalonJNI.GetQuadBpin(m_handle);
  }

  /**
   * @return IO level of QUAD Index pin.
   */
  public int getPinStateQuadIdx() {
    return CanTalonJNI.GetQuadIdxpin(m_handle);
  }

  public void setAnalogPosition(int newPosition){
    setParameter(CanTalonJNI.param_t.eAinPosition, (double)newPosition);
  }
  /**
   * Get the current analog in position, regardless of whether it is the current
   * feedback device.
   *
   * @return The 24bit analog position. The bottom ten bits is the ADC (0 -
   *         1023) on the analog pin of the Talon. The upper 14 bits tracks the
   *         overflows and underflows (continuous sensor).
   */
  public int getAnalogInPosition() {
    return CanTalonJNI.GetAnalogInWithOv(m_handle);
  }

  /**
   * Get the current analog in position, regardless of whether it is the current
   * feedback device.
   *$
   * @return The ADC (0 - 1023) on analog pin of the Talon.
   */
  public int getAnalogInRaw() {
    return getAnalogInPosition() & 0x3FF;
  }

  /**
   * Get the current encoder velocity, regardless of whether it is the current
   * feedback device.
   *
   * @return The current speed of the analog in device.
   */
  public int getAnalogInVelocity() {
    return CanTalonJNI.GetAnalogInVel(m_handle);
  }

  /**
   * Get the current difference between the setpoint and the sensor value.
   *
   * @return The error, in whatever units are appropriate.
   */
  public int getClosedLoopError() {
    /* retrieve the closed loop error in native units */
    return CanTalonJNI.GetCloseLoopErr(m_handle);
  }
  /**
   * Set the allowable closed loop error.
   * @param allowableCloseLoopError allowable closed loop error for selected profile.
   *             mA for Curent closed loop.
   *             Talon Native Units for position and velocity.
   */
  public void setAllowableClosedLoopErr(int allowableCloseLoopError)
  {
    if(m_profile == 0){
      setParameter(CanTalonJNI.param_t.eProfileParamSlot0_AllowableClosedLoopErr, (double)allowableCloseLoopError);
    }else{
      setParameter(CanTalonJNI.param_t.eProfileParamSlot1_AllowableClosedLoopErr, (double)allowableCloseLoopError);
    }
  }

  // Returns true if limit switch is closed. false if open.
  public boolean isFwdLimitSwitchClosed() {
    return (CanTalonJNI.GetLimitSwitchClosedFor(m_handle) == 0) ? true : false;
  }

  // Returns true if limit switch is closed. false if open.
  public boolean isRevLimitSwitchClosed() {
    return (CanTalonJNI.GetLimitSwitchClosedRev(m_handle) == 0) ? true : false;
  }

  // Returns true if break is enabled during neutral. false if coast.
  public boolean getBrakeEnableDuringNeutral() {
    return (CanTalonJNI.GetBrakeIsEnabled(m_handle) == 0) ? false : true;
  }

  /**
   * Configure how many codes per revolution are generated by your encoder.
   *
   * @param codesPerRev The number of counts per revolution.
   */
  public void configEncoderCodesPerRev(int codesPerRev) {
    /* first save the scalar so that all getters/setter work as the user expects */
    m_codesPerRev = codesPerRev;
    /* next send the scalar to the Talon over CAN.  This is so that the Talon can report
      it to whoever needs it, like the webdash.  Don't bother checking the return,
      this is only for instrumentation and is not necessary for Talon functionality. */
    setParameter(CanTalonJNI.param_t.eNumberEncoderCPR, m_codesPerRev);
  }
  /**
   * Configure the number of turns on the potentiometer.
   *
   * @param turns The number of turns of the potentiometer.
   */
  public void configPotentiometerTurns(int turns) {
    /* first save the scalar so that all getters/setter work as the user expects */
    m_numPotTurns = turns;
    /* next send the scalar to the Talon over CAN.  This is so that the Talon can report
      it to whoever needs it, like the webdash.  Don't bother checking the return,
      this is only for instrumentation and is not necessary for Talon functionality. */
    setParameter(CanTalonJNI.param_t.eNumberPotTurns, m_numPotTurns);
  }
  /**
   * Returns temperature of Talon, in degrees Celsius.
   */
  public double getTemperature() {
    return CanTalonJNI.GetTemp(m_handle);
  }

  /**
   * Returns the current going through the Talon, in Amperes.
   */
  public double getOutputCurrent() {
    return CanTalonJNI.GetCurrent(m_handle);
  }

  /**
   * @return The voltage being output by the Talon, in Volts.
   */
  public double getOutputVoltage() {
    return getBusVoltage() * (double) CanTalonJNI.GetAppliedThrottle(m_handle) / 1023.0;
  }

  /**
   * @return The voltage at the battery terminals of the Talon, in Volts.
   */
  public double getBusVoltage() {
    return CanTalonJNI.GetBatteryV(m_handle);
  }

  /**
   * TODO documentation (see CANJaguar.java)
   *
   * @return The position of the sensor currently providing feedback. When using
   *         analog sensors, 0 units corresponds to 0V, 1023 units corresponds
   *         to 3.3V When using an analog encoder (wrapping around 1023 to 0 is
   *         possible) the units are still 3.3V per 1023 units. When using
   *         quadrature, each unit is a quadrature edge (4X) mode.
   */
  public double getPosition() {
    return ScaleNativeUnitsToRotations(m_feedbackDevice,CanTalonJNI.GetSensorPosition(m_handle));
  }

  public void setPosition(double pos) {
    int nativePos = ScaleRotationsToNativeUnits(m_feedbackDevice,pos);
    CanTalonJNI.SetSensorPosition(m_handle, nativePos);
  }

  /**
   * TODO documentation (see CANJaguar.java)
   *
   * @return The speed of the sensor currently providing feedback.
   *
   *         The speed units will be in the sensor's native ticks per 100ms.
   *
   *         For analog sensors, 3.3V corresponds to 1023 units. So a speed of
   *         200 equates to ~0.645 dV per 100ms or 6.451 dV per second. If this
   *         is an analog encoder, that likely means 1.9548 rotations per sec.
   *         For quadrature encoders, each unit corresponds a quadrature edge
   *         (4X). So a 250 count encoder will produce 1000 edge events per
   *         rotation. An example speed of 200 would then equate to 20% of a
   *         rotation per 100ms, or 10 rotations per second.
   */
  public double getSpeed() {
    return ScaleNativeUnitsToRpm(m_feedbackDevice,CanTalonJNI.GetSensorVelocity(m_handle));
  }

  public TalonControlMode getControlMode() {
    return m_controlMode;
  }

  public void setControlMode(int mode) {
    TalonControlMode tcm = TalonControlMode.valueOf(mode);
    if(tcm != null)
      changeControlMode(tcm);
  }

  /**
   * Fixup the m_controlMode so set() serializes the correct demand value. Also
   * fills the modeSelecet in the control frame to disabled.
   *$
   * @param controlMode Control mode to ultimately enter once user calls set().
   * @see #set
   */
  private void applyControlMode(TalonControlMode controlMode) {
    m_controlMode = controlMode;
    if (controlMode == TalonControlMode.Disabled)
      m_controlEnabled = false;
    // Disable until set() is called.
    CanTalonJNI.SetModeSelect(m_handle, TalonControlMode.Disabled.value);

    UsageReporting.report(tResourceType.kResourceType_CANTalonSRX, m_deviceNumber + 1,
        controlMode.value);
  }

  public void changeControlMode(TalonControlMode controlMode) {
    if (m_controlMode == controlMode) {
      /* we already are in this mode, don't perform disable workaround */
    } else {
      applyControlMode(controlMode);
    }
  }

  public void setFeedbackDevice(FeedbackDevice device) {
    /* save the selection so that future setters/getters know which scalars to apply */
    m_feedbackDevice = device;
    /* pass feedback to actual CAN frame */
    CanTalonJNI.SetFeedbackDeviceSelect(m_handle, device.value);
  }

  public void setStatusFrameRateMs(StatusFrameRate stateFrame, int periodMs) {
    CanTalonJNI.SetStatusFrameRate(m_handle, stateFrame.value, periodMs);
  }

  public void enableControl() {
    changeControlMode(m_controlMode);
    m_controlEnabled = true;
  }

  public void enable() {
    enableControl();
  }

  public void disableControl() {
    CanTalonJNI.SetModeSelect(m_handle, TalonControlMode.Disabled.value);
    m_controlEnabled = false;
  }

  public boolean isControlEnabled() {
    return m_controlEnabled;
  }

  /**
   * Get the current proportional constant.
   *
   * @return double proportional constant for current profile.
   */
  public double getP() {
    // if(!(m_controlMode.equals(ControlMode.Position) ||
    // m_controlMode.equals(ControlMode.Speed))) {
    // throw new
    // IllegalStateException("PID mode only applies in Position and Speed modes.");
    // }

    // Update the information that we have.
    if (m_profile == 0)
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot0_P.value);
    else
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot1_P.value);

    // Briefly wait for new values from the Talon.
    Timer.delay(kDelayForSolicitedSignals);

    return CanTalonJNI.GetPgain(m_handle, m_profile);
  }

  public double getI() {
    // if(!(m_controlMode.equals(ControlMode.Position) ||
    // m_controlMode.equals(ControlMode.Speed))) {
    // throw new
    // IllegalStateException("PID mode only applies in Position and Speed modes.");
    // }

    // Update the information that we have.
    if (m_profile == 0)
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot0_I.value);
    else
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot1_I.value);

    // Briefly wait for new values from the Talon.
    Timer.delay(kDelayForSolicitedSignals);

    return CanTalonJNI.GetIgain(m_handle, m_profile);
  }

  public double getD() {
    // if(!(m_controlMode.equals(ControlMode.Position) ||
    // m_controlMode.equals(ControlMode.Speed))) {
    // throw new
    // IllegalStateException("PID mode only applies in Position and Speed modes.");
    // }

    // Update the information that we have.
    if (m_profile == 0)
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot0_D.value);
    else
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot1_D.value);

    // Briefly wait for new values from the Talon.
    Timer.delay(kDelayForSolicitedSignals);

    return CanTalonJNI.GetDgain(m_handle, m_profile);
  }

  public double getF() {
    // if(!(m_controlMode.equals(ControlMode.Position) ||
    // m_controlMode.equals(ControlMode.Speed))) {
    // throw new
    // IllegalStateException("PID mode only applies in Position and Speed modes.");
    // }

    // Update the information that we have.
    if (m_profile == 0)
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot0_F.value);
    else
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot1_F.value);

    // Briefly wait for new values from the Talon.
    Timer.delay(kDelayForSolicitedSignals);

    return CanTalonJNI.GetFgain(m_handle, m_profile);
  }

  public double getIZone() {
    // if(!(m_controlMode.equals(ControlMode.Position) ||
    // m_controlMode.equals(ControlMode.Speed))) {
    // throw new
    // IllegalStateException("PID mode only applies in Position and Speed modes.");
    // }

    // Update the information that we have.
    if (m_profile == 0)
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot0_IZone.value);
    else
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot1_IZone.value);

    // Briefly wait for new values from the Talon.
    Timer.delay(kDelayForSolicitedSignals);

    return CanTalonJNI.GetIzone(m_handle, m_profile);
  }

  /**
   * Get the closed loop ramp rate for the current profile.
   *
   * Limits the rate at which the throttle will change. Only affects position
   * and speed closed loop modes.
   *
   * @return rampRate Maximum change in voltage, in volts / sec.
   * @see #setProfile For selecting a certain profile.
   */
  public double getCloseLoopRampRate() {
    // if(!(m_controlMode.equals(ControlMode.Position) ||
    // m_controlMode.equals(ControlMode.Speed))) {
    // throw new
    // IllegalStateException("PID mode only applies in Position and Speed modes.");
    // }

    // Update the information that we have.
    if (m_profile == 0)
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot0_CloseLoopRampRate.value);
    else
      CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eProfileParamSlot1_CloseLoopRampRate.value);

    // Briefly wait for new values from the Talon.
    Timer.delay(kDelayForSolicitedSignals);

    double throttlePerMs = CanTalonJNI.GetCloseLoopRampRate(m_handle, m_profile);
    return throttlePerMs / 1023.0 * 12.0 * 1000.0;
  }

  /**
   * @return The version of the firmware running on the Talon
   */
  public long GetFirmwareVersion() {

    // Update the information that we have.
    CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.eFirmVers.value);

    // Briefly wait for new values from the Talon.
    Timer.delay(kDelayForSolicitedSignals);

    return CanTalonJNI.GetParamResponseInt32(m_handle, CanTalonJNI.param_t.eFirmVers.value);
  }

  public long GetIaccum() {

    // Update the information that we have.
    CanTalonJNI.RequestParam(m_handle, CanTalonJNI.param_t.ePidIaccum.value);

    // Briefly wait for new values from the Talon.
    Timer.delay(kDelayForSolicitedSignals);

    return CanTalonJNI.GetParamResponseInt32(m_handle, CanTalonJNI.param_t.ePidIaccum.value);
  }

  /**
   * Set the proportional value of the currently selected profile.
   *
   * @param p Proportional constant for the currently selected PID profile.
   * @see #setProfile For selecting a certain profile.
   */
  public void setP(double p) {
    CanTalonJNI.SetPgain(m_handle, m_profile, p);
  }

  /**
   * Set the integration constant of the currently selected profile.
   *
   * @param i Integration constant for the currently selected PID profile.
   * @see #setProfile For selecting a certain profile.
   */
  public void setI(double i) {
    CanTalonJNI.SetIgain(m_handle, m_profile, i);
  }

  /**
   * Set the derivative constant of the currently selected profile.
   *
   * @param d Derivative constant for the currently selected PID profile.
   * @see #setProfile For selecting a certain profile.
   */
  public void setD(double d) {
    CanTalonJNI.SetDgain(m_handle, m_profile, d);
  }

  /**
   * Set the feedforward value of the currently selected profile.
   *
   * @param f Feedforward constant for the currently selected PID profile.
   * @see #setProfile For selecting a certain profile.
   */
  public void setF(double f) {
    CanTalonJNI.SetFgain(m_handle, m_profile, f);
  }

  /**
   * Set the integration zone of the current Closed Loop profile.
   *
   * Whenever the error is larger than the izone value, the accumulated
   * integration error is cleared so that high errors aren't racked up when at
   * high errors. An izone value of 0 means no difference from a standard PIDF
   * loop.
   *
   * @param izone Width of the integration zone.
   * @see #setProfile For selecting a certain profile.
   */
  public void setIZone(int izone) {
    CanTalonJNI.SetIzone(m_handle, m_profile, izone);
  }

  /**
   * Set the closed loop ramp rate for the current profile.
   *
   * Limits the rate at which the throttle will change. Only affects position
   * and speed closed loop modes.
   *
   * @param rampRate Maximum change in voltage, in volts / sec.
   * @see #setProfile For selecting a certain profile.
   */
  public void setCloseLoopRampRate(double rampRate) {
    // CanTalonJNI takes units of Throttle (0 - 1023) / 1ms.
    int rate = (int) (rampRate * 1023.0 / 12.0 / 1000.0);
    CanTalonJNI.SetCloseLoopRampRate(m_handle, m_profile, rate);
  }

  /**
   * Set the voltage ramp rate for the current profile.
   *
   * Limits the rate at which the throttle will change. Affects all modes.
   *
   * @param rampRate Maximum change in voltage, in volts / sec.
   */
  public void setVoltageRampRate(double rampRate) {
    // CanTalonJNI takes units of Throttle (0 - 1023) / 10ms.
    int rate = (int) (rampRate * 1023.0 / 12.0 / 100.0);
    CanTalonJNI.SetRampThrottle(m_handle, rate);
  }

  public void setVoltageCompensationRampRate(double rampRate) {
    CanTalonJNI.SetVoltageCompensationRate(m_handle, rampRate / 1000);
  }
  /**
   * Clear the accumulator for I gain.
   */
  public void ClearIaccum() {
    CanTalonJNI.SetParam(m_handle, CanTalonJNI.param_t.ePidIaccum.value, 0);
  }

  /**
   * Sets control values for closed loop control.
   *
   * @param p Proportional constant.
   * @param i Integration constant.
   * @param d Differential constant.
   * @param f Feedforward constant.
   * @param izone Integration zone -- prevents accumulation of integration error
   *        with large errors. Setting this to zero will ignore any izone stuff.
   * @param closeLoopRampRate Closed loop ramp rate. Maximum change in voltage,
   *        in volts / sec.
   * @param profile which profile to set the pid constants for. You can have two
   *        profiles, with values of 0 or 1, allowing you to keep a second set
   *        of values on hand in the talon. In order to switch profiles without
   *        recalling setPID, you must call setProfile().
   */
  public void setPID(double p, double i, double d, double f, int izone, double closeLoopRampRate,
      int profile) {
    if (profile != 0 && profile != 1)
      throw new IllegalArgumentException("Talon PID profile must be 0 or 1.");
    m_profile = profile;
    setProfile(profile);
    setP(p);
    setI(i);
    setD(d);
    setF(f);
    setIZone(izone);
    setCloseLoopRampRate(closeLoopRampRate);
  }

  public void setPID(double p, double i, double d) {
    setPID(p, i, d, 0, 0, 0, m_profile);
  }

  /**
   * @return The latest value set using set().
   */
  public double getSetpoint() {
    return m_setPoint;
  }

  /**
   * Select which closed loop profile to use, and uses whatever PIDF gains and
   * the such that are already there.
   */
  public void setProfile(int profile) {
    if (profile != 0 && profile != 1)
      throw new IllegalArgumentException("Talon PID profile must be 0 or 1.");
    m_profile = profile;
    CanTalonJNI.SetProfileSlotSelect(m_handle, m_profile);
  }

  /**
   * Common interface for stopping a motor.
   *
   * @deprecated Use disableControl instead.
   */
  @Override
  public void stopMotor() {
    disableControl();
    m_stopped = true;
  }

  @Override
  public void disable() {
    disableControl();
  }

  public int getDeviceID() {
    return m_deviceNumber;
  }

  // TODO: Documentation for all these accessors/setters for misc. stuff.
  public void clearIAccum() {
    CanTalonJNI.SetParam(m_handle, CanTalonJNI.param_t.ePidIaccum.value, 0);
  }

  public void setForwardSoftLimit(double forwardLimit) {
    int nativeLimitPos = ScaleRotationsToNativeUnits(m_feedbackDevice,forwardLimit);
    CanTalonJNI.SetForwardSoftLimit(m_handle, nativeLimitPos);
  }

  public int getForwardSoftLimit() {
    return CanTalonJNI.GetForwardSoftLimit(m_handle);
  }

  public void enableForwardSoftLimit(boolean enable) {
    CanTalonJNI.SetForwardSoftEnable(m_handle, enable ? 1 : 0);
  }

  public boolean isForwardSoftLimitEnabled() {
    return (CanTalonJNI.GetForwardSoftEnable(m_handle) == 0) ? false : true;
  }

  public void setReverseSoftLimit(double reverseLimit) {
    int nativeLimitPos = ScaleRotationsToNativeUnits(m_feedbackDevice,reverseLimit);
    CanTalonJNI.SetReverseSoftLimit(m_handle, nativeLimitPos);
  }

  public int getReverseSoftLimit() {
    return CanTalonJNI.GetReverseSoftLimit(m_handle);
  }

  public void enableReverseSoftLimit(boolean enable) {
    CanTalonJNI.SetReverseSoftEnable(m_handle, enable ? 1 : 0);
  }

  public boolean isReverseSoftLimitEnabled() {
    return (CanTalonJNI.GetReverseSoftEnable(m_handle) == 0) ? false : true;
  }
  /**
   * Configure the maximum voltage that the Jaguar will ever output.
   *
   * This can be used to limit the maximum output voltage in all modes so that
   * motors which cannot withstand full bus voltage can be used safely.
   *
   * @param voltage The maximum voltage output by the Jaguar.
   */
  public void configMaxOutputVoltage(double voltage) {
    /* config peak throttle when in closed-loop mode in the fwd and rev direction. */
    configPeakOutputVoltage(voltage, -voltage);
  }

  public void configPeakOutputVoltage(double forwardVoltage,double reverseVoltage) {
    /* bounds checking */
    if(forwardVoltage > 12)
      forwardVoltage = 12;
    else if(forwardVoltage < 0)
      forwardVoltage = 0;
    if(reverseVoltage > 0)
      reverseVoltage = 0;
    else if(reverseVoltage < -12)
      reverseVoltage = -12;
    /* config calls */
    setParameter(CanTalonJNI.param_t.ePeakPosOutput,1023*forwardVoltage/12.0);
    setParameter(CanTalonJNI.param_t.ePeakNegOutput,1023*reverseVoltage/12.0);
  }
  public void configNominalOutputVoltage(double forwardVoltage,double reverseVoltage) {
    /* bounds checking */
    if(forwardVoltage > 12)
      forwardVoltage = 12;
    else if(forwardVoltage < 0)
      forwardVoltage = 0;
    if(reverseVoltage > 0)
      reverseVoltage = 0;
    else if(reverseVoltage < -12)
      reverseVoltage = -12;
    /* config calls */
    setParameter(CanTalonJNI.param_t.eNominalPosOutput,1023*forwardVoltage/12.0);
    setParameter(CanTalonJNI.param_t.eNominalNegOutput,1023*reverseVoltage/12.0);
  }
  /**
   * General set frame.  Since the parameter is a general integral type, this can
   * be used for testing future features.
   */
  public void setParameter(CanTalonJNI.param_t paramEnum, double value){
    CanTalonJNI.SetParam(m_handle,paramEnum.value,value);
  }
  /**
   * General get frame.  Since the parameter is a general integral type, this can
   * be used for testing future features.
   */
  public double getParameter(CanTalonJNI.param_t paramEnum) {
    /* transmit a request for this param */
    CanTalonJNI.RequestParam(m_handle, paramEnum.value);
    /* Briefly wait for new values from the Talon. */
    Timer.delay(kDelayForSolicitedSignals);
    /* poll out latest response value */
    return CanTalonJNI.GetParamResponse(m_handle, paramEnum.value);
  }
  public void clearStickyFaults() {
    CanTalonJNI.ClearStickyFaults(m_handle);
  }

  public void enableLimitSwitch(boolean forward, boolean reverse) {
    int mask = 4 + (forward ? 1 : 0) * 2 + (reverse ? 1 : 0);
    CanTalonJNI.SetOverrideLimitSwitchEn(m_handle, mask);
  }

  /**
   * Configure the fwd limit switch to be normally open or normally closed.
   * Talon will disable momentarilly if the Talon's current setting is
   * dissimilar to the caller's requested setting.
   *
   * Since Talon saves setting to flash this should only affect a given Talon
   * initially during robot install.
   *
   * @param normallyOpen true for normally open. false for normally closed.
   */
  public void ConfigFwdLimitSwitchNormallyOpen(boolean normallyOpen) {
    CanTalonJNI.SetParam(m_handle, CanTalonJNI.param_t.eOnBoot_LimitSwitch_Forward_NormallyClosed.value,
            normallyOpen ? 0 : 1);
  }

  /**
   * Configure the rev limit switch to be normally open or normally closed.
   * Talon will disable momentarilly if the Talon's current setting is
   * dissimilar to the caller's requested setting.
   *
   * Since Talon saves setting to flash this should only affect a given Talon
   * initially during robot install.
   *
   * @param normallyOpen true for normally open. false for normally closed.
   */
  public void ConfigRevLimitSwitchNormallyOpen(boolean normallyOpen) {
    CanTalonJNI.SetParam(m_handle, CanTalonJNI.param_t.eOnBoot_LimitSwitch_Reverse_NormallyClosed.value,
            normallyOpen ? 0 : 1);
  }

  public void enableBrakeMode(boolean brake) {
    CanTalonJNI.SetOverrideBrakeType(m_handle, brake ? 2 : 1);
  }

  public int getFaultOverTemp() {
    return CanTalonJNI.GetFault_OverTemp(m_handle);
  }

  public int getFaultUnderVoltage() {
    return CanTalonJNI.GetFault_UnderVoltage(m_handle);
  }

  public int getFaultForLim() {
    return CanTalonJNI.GetFault_ForLim(m_handle);
  }

  public int getFaultRevLim() {
    return CanTalonJNI.GetFault_RevLim(m_handle);
  }

  public int getFaultHardwareFailure() {
    return CanTalonJNI.GetFault_HardwareFailure(m_handle);
  }

  public int getFaultForSoftLim() {
    return CanTalonJNI.GetFault_ForSoftLim(m_handle);
  }

  public int getFaultRevSoftLim() {
    return CanTalonJNI.GetFault_RevSoftLim(m_handle);
  }

  public int getStickyFaultOverTemp() {
    return CanTalonJNI.GetStckyFault_OverTemp(m_handle);
  }

  public int getStickyFaultUnderVoltage() {
    return CanTalonJNI.GetStckyFault_UnderVoltage(m_handle);
  }

  public int getStickyFaultForLim() {
    return CanTalonJNI.GetStckyFault_ForLim(m_handle);
  }

  public int getStickyFaultRevLim() {
    return CanTalonJNI.GetStckyFault_RevLim(m_handle);
  }

  public int getStickyFaultForSoftLim() {
    return CanTalonJNI.GetStckyFault_ForSoftLim(m_handle);
  }

  public int getStickyFaultRevSoftLim() {
    return CanTalonJNI.GetStckyFault_RevSoftLim(m_handle);
  }
  /**
   * @return Number of native units per rotation if scaling info is available.
   *              Zero if scaling information is not available.
   */
  double GetNativeUnitsPerRotationScalar(FeedbackDevice devToLookup)
  {
    double retval = 0;
    boolean scalingAvail = false;
    switch(devToLookup){
      case QuadEncoder:
      { /* When caller wants to lookup Quadrature, the QEI may be in 1x if the selected feedback is edge counter.
         * Additionally if the quadrature source is the CTRE Mag encoder, then the CPR is known.
         * This is nice in that the calling app does not require knowing the CPR at all.
         * So do both checks here.
         */
        int qeiPulsePerCount = 4; /* default to 4x */
        switch(m_feedbackDevice){
          case CtreMagEncoder_Relative:
          case CtreMagEncoder_Absolute:
            /* we assume the quadrature signal comes from the MagEnc,
                of which we know the CPR already */
            retval = kNativePwdUnitsPerRotation;
            scalingAvail = true;
            break;
          case EncRising: /* Talon's QEI is setup for 1x, so perform 1x math */
          case EncFalling:
            qeiPulsePerCount = 1;
            break;
          case QuadEncoder: /* Talon's QEI is 4x */
          default: /* pulse width and everything else, assume its regular quad use. */
            break;
      }
      if(scalingAvail){
        /* already deduced the scalar above, we're done. */
      }else{
        /* we couldn't deduce the scalar just based on the selection */
        if(0 == m_codesPerRev){
          /* caller has never set the CPR.  Most likely caller
              is just using engineering units so fall to the
              bottom of this func.*/
        }else{
          /* Talon expects PPR units */
          retval = 4 * m_codesPerRev;
          scalingAvail = true;
        }
      }
    }    break;
    case EncRising:
    case EncFalling:
      if(0 == m_codesPerRev){
        /* caller has never set the CPR.  Most likely caller
            is just using engineering units so fall to the
            bottom of this func.*/
      }else{
        /* Talon expects PPR units */
        retval = 1 * m_codesPerRev;
        scalingAvail = true;
      }
      break;
    case AnalogPot:
    case AnalogEncoder:
      if(0 == m_numPotTurns){
        /* caller has never set the CPR.  Most likely caller
            is just using engineering units so fall to the
            bottom of this func.*/
      }else {
        retval = (double)kNativeAdcUnitsPerRotation / m_numPotTurns;
        scalingAvail = true;
      }
      break;
    case CtreMagEncoder_Relative:
    case CtreMagEncoder_Absolute:
    case PulseWidth:
      retval = kNativePwdUnitsPerRotation;
      scalingAvail = true;
      break;
    }
    /* if scaling info is not available give caller zero */
    if(false == scalingAvail)
      return 0;
    return retval;
  }
  /**
   * @param fullRotations     double precision value representing number of rotations of selected feedback sensor.
   *                            If user has never called the config routine for the selected sensor, then the caller
   *                            is likely passing rotations in engineering units already, in which case it is returned
   *                            as is.
   *                            @see configPotentiometerTurns
   *                            @see configEncoderCodesPerRev
   * @return fullRotations in native engineering units of the Talon SRX firmware.
   */
  int ScaleRotationsToNativeUnits(FeedbackDevice devToLookup, double fullRotations)
  {
    /* first assume we don't have config info, prep the default return */
    int retval = (int)fullRotations;
    /* retrieve scaling info */
    double scalar = GetNativeUnitsPerRotationScalar(devToLookup);
    /* apply scalar if its available */
    if(scalar > 0)
      retval = (int)(fullRotations*scalar);
    return retval;
  }
  /**
   * @param rpm     double precision value representing number of rotations per minute of selected feedback sensor.
   *                            If user has never called the config routine for the selected sensor, then the caller
   *                            is likely passing rotations in engineering units already, in which case it is returned
   *                            as is.
   *                            @see configPotentiometerTurns
   *                            @see configEncoderCodesPerRev
   * @return sensor velocity in native engineering units of the Talon SRX firmware.
   */
  int ScaleVelocityToNativeUnits(FeedbackDevice devToLookup, double rpm)
  {
    /* first assume we don't have config info, prep the default return */
    int retval = (int)rpm;
    /* retrieve scaling info */
    double scalar = GetNativeUnitsPerRotationScalar(devToLookup);
    /* apply scalar if its available */
    if(scalar > 0)
      retval = (int)(rpm * kMinutesPer100msUnit * scalar);
    return retval;
  }
  /**
   * @param nativePos     integral position of the feedback sensor in native Talon SRX units.
   *                            If user has never called the config routine for the selected sensor, then the return
   *                            will be in TALON SRX units as well to match the behavior in the 2015 season.
   *                            @see configPotentiometerTurns
   *                            @see configEncoderCodesPerRev
   * @return double precision number of rotations, unless config was never performed.
   */
  double ScaleNativeUnitsToRotations(FeedbackDevice devToLookup, int nativePos)
  {
    /* first assume we don't have config info, prep the default return */
    double retval = (double)nativePos;
    /* retrieve scaling info */
    double scalar = GetNativeUnitsPerRotationScalar(devToLookup);
    /* apply scalar if its available */
    if(scalar > 0)
      retval = ((double)nativePos) / scalar;
    return retval;
  }
  /**
   * @param nativeVel     integral velocity of the feedback sensor in native Talon SRX units.
   *                            If user has never called the config routine for the selected sensor, then the return
   *                            will be in TALON SRX units as well to match the behavior in the 2015 season.
   *                            @see configPotentiometerTurns
   *                            @see configEncoderCodesPerRev
   * @return double precision of sensor velocity in RPM, unless config was never performed.
   */
  double ScaleNativeUnitsToRpm(FeedbackDevice devToLookup, long nativeVel)
  {
    /* first assume we don't have config info, prep the default return */
    double retval = (double)nativeVel;
    /* retrieve scaling info */
    double scalar = GetNativeUnitsPerRotationScalar(devToLookup);
    /* apply scalar if its available */
    if(scalar > 0)
      retval = (double)(nativeVel) / (scalar*kMinutesPer100msUnit);
    return retval;
  }
  /**
   * Enables Talon SRX to automatically zero the Sensor Position whenever an
   * edge is detected on the index signal.
   * @param enable         boolean input, pass true to enable feature or false to disable.
   * @param risingEdge     boolean input, pass true to clear the position on rising edge,
   *                    pass false to clear the position on falling edge.
   */
  public void enableZeroSensorPositionOnIndex(boolean enable, boolean risingEdge) {
    if(enable){
      /* enable the feature, update the edge polarity first to ensure
        it is correct before the feature is enabled. */
      setParameter(CanTalonJNI.param_t.eQuadIdxPolarity,risingEdge ? 1 : 0);
      setParameter(CanTalonJNI.param_t.eClearPositionOnIdx,1);
    }else{
      /* disable the feature first, then update the edge polarity. */
      setParameter(CanTalonJNI.param_t.eClearPositionOnIdx,0);
      setParameter(CanTalonJNI.param_t.eQuadIdxPolarity,risingEdge ? 1 : 0);
    }
  }
  /**
   * Calling application can opt to speed up the handshaking between the robot API and the Talon to increase the
   * download rate of the Talon's Motion Profile.  Ideally the period should be no more than half the period
   * of a trajectory point.
   */
  public void changeMotionControlFramePeriod(int periodMs) {
    CanTalonJNI.ChangeMotionControlFramePeriod(m_handle, periodMs);
  }

  /**
   * Clear the buffered motion profile in both Talon RAM (bottom), and in the API (top).
   * Be sure to check getMotionProfileStatus() to know when the buffer is actually cleared.
   */
  public void clearMotionProfileTrajectories() {
    CanTalonJNI.ClearMotionProfileTrajectories(m_handle);
  }
  /**
   * Retrieve just the buffer count for the api-level (top) buffer.
   * This routine performs no CAN or data structure lookups, so its fast and ideal
   * if caller needs to quickly poll the progress of trajectory points being emptied
   * into Talon's RAM. Otherwise just use GetMotionProfileStatus.
   * @return number of trajectory points in the top buffer.
   */
  public int getMotionProfileTopLevelBufferCount() {
    return CanTalonJNI.GetMotionProfileTopLevelBufferCount(m_handle);
  }
  /**
   * Push another trajectory point into the top level buffer (which is emptied into
   * the Talon's bottom buffer as room allows).
   * @param targPos servo position in native Talon units (sensor units).
   * @param targVel velocity to feed-forward in native Talon units (sensor units per 100ms).
   * @param profileSlotSelect  which slot to pull PIDF gains from.  Currently supports 0 or 1.
   * @param timeDurMs time in milliseconds of how long to apply this point.
   * @param velOnly  set to nonzero to signal Talon that only the feed-foward velocity should be
   *                 used, i.e. do not perform PID on position.  This is equivalent to setting
   *                 PID gains to zero, but much more efficient and synchronized to MP.
   * @param isLastPoint  set to nonzero to signal Talon to keep processing this trajectory point,
   *                     instead of jumping to the next one when timeDurMs expires.  Otherwise
   *                     MP executer will eventuall see an empty buffer after the last point expires,
   *                     causing it to assert the IsUnderRun flag.  However this may be desired
   *                     if calling application nevers wants to terminate the MP.
   * @param zeroPos  set to nonzero to signal Talon to "zero" the selected position sensor before executing
   *                 this trajectory point.  Typically the first point should have this set only thus allowing
   *                 the remainder of the MP positions to be relative to zero.
   * @return CTR_OKAY if trajectory point push ok. CTR_BufferFull if buffer is full due to kMotionProfileTopBufferCapacity.
   */
  public boolean pushMotionProfileTrajectory(TrajectoryPoint trajPt) {
    /* check if there is room */
    if(isMotionProfileTopLevelBufferFull())
      return false;
    /* convert position and velocity to native units */
    int targPos  = ScaleRotationsToNativeUnits(m_feedbackDevice, trajPt.position);
    int targVel = ScaleVelocityToNativeUnits(m_feedbackDevice, trajPt.velocity);
    /* bounds check signals that require it */
    int profileSlotSelect = (trajPt.profileSlotSelect > 0) ? 1 : 0;
    int timeDurMs = trajPt.timeDurMs;
    /* cap time to [0ms, 255ms], 0 and 1 are both interpreted as 1ms. */
    if(timeDurMs > 255)
      timeDurMs = 255;
    if(timeDurMs < 0)
      timeDurMs = 0;
    /* send it to the top level buffer */
    CanTalonJNI.PushMotionProfileTrajectory(m_handle, targPos, targVel, profileSlotSelect, timeDurMs, trajPt.velocityOnly ? 1 : 0, trajPt.isLastPoint ? 1 : 0, trajPt.zeroPos ? 1 : 0);
    return true;
  }
  /**
   * @return true if api-level (top) buffer is full.
   */
  public boolean isMotionProfileTopLevelBufferFull() {
    return CanTalonJNI.IsMotionProfileTopLevelBufferFull(m_handle);
  }
  /**
   * This must be called periodically to funnel the trajectory points from the API's top level buffer to
   * the Talon's bottom level buffer.  Recommendation is to call this twice as fast as the executation rate of the motion profile.
   * So if MP is running with 20ms trajectory points, try calling this routine every 10ms.  All motion profile functions are thread-safe
   * through the use of a mutex, so there is no harm in having the caller utilize threading.
   */
  public void processMotionProfileBuffer() {
    CanTalonJNI.ProcessMotionProfileBuffer(m_handle);
  }
  /**
   * Retrieve all Motion Profile status information.
   * Since this all comes from one CAN frame, its ideal to have one routine to retrieve the frame once and decode everything.
   * @param [out] motionProfileStatus contains all progress information on the currently running MP.  Caller should
   *              must instantiate the motionProfileStatus object first then pass into this routine to be filled.
   */
  public void getMotionProfileStatus(MotionProfileStatus motionProfileStatus) {
    CanTalonJNI.GetMotionProfileStatus(m_handle, this, motionProfileStatus);
  }

  /**
   * Internal method to set the contents.
   */
  protected void setMotionProfileStatusFromJNI(MotionProfileStatus motionProfileStatus, int flags, int profileSlotSelect, int targPos, int targVel, int topBufferRem, int topBufferCnt, int btmBufferCnt, int outputEnable) {
    motionProfileStatus.topBufferRem = topBufferRem;
    motionProfileStatus.topBufferCnt = topBufferCnt;
    motionProfileStatus.btmBufferCnt = btmBufferCnt;
    motionProfileStatus.hasUnderrun =              ((flags & CanTalonJNI.kMotionProfileFlag_HasUnderrun)>0)     ? true :false;
    motionProfileStatus.isUnderrun  =              ((flags & CanTalonJNI.kMotionProfileFlag_IsUnderrun)>0)      ? true :false;
    motionProfileStatus.activePointValid =         ((flags & CanTalonJNI.kMotionProfileFlag_ActTraj_IsValid)>0) ? true :false;
    motionProfileStatus.activePoint.isLastPoint =  ((flags & CanTalonJNI.kMotionProfileFlag_ActTraj_IsLast)>0)  ? true :false;
    motionProfileStatus.activePoint.velocityOnly = ((flags & CanTalonJNI.kMotionProfileFlag_ActTraj_VelOnly)>0) ? true :false;
    motionProfileStatus.activePoint.position = ScaleNativeUnitsToRotations(m_feedbackDevice, targPos);
    motionProfileStatus.activePoint.velocity = ScaleNativeUnitsToRpm(m_feedbackDevice, targVel);
    motionProfileStatus.activePoint.profileSlotSelect = profileSlotSelect;
    motionProfileStatus.outputEnable = SetValueMotionProfile.valueOf(outputEnable);
    motionProfileStatus.activePoint.zeroPos = false; // this signal is only used sending pts to Talon
    motionProfileStatus.activePoint.timeDurMs = 0;   // this signal is only used sending pts to Talon
  }

  /**
   * Clear the hasUnderrun flag in Talon's Motion Profile Executer when MPE is ready for another point,
   * but the low level buffer is empty.
   *
   * Once the Motion Profile Executer sets the hasUnderrun flag, it stays set until
   * Robot Application clears it with this routine, which ensures Robot Application
   * gets a chance to instrument or react.  Caller could also check the isUnderrun flag
   * which automatically clears when fault condition is removed.
   */
  public void clearMotionProfileHasUnderrun() {
    setParameter(CanTalonJNI.param_t.eMotionProfileHasUnderrunErr, 0);
  }
  @Override
  public void setExpiration(double timeout) {
    m_safetyHelper.setExpiration(timeout);
  }

  @Override
  public double getExpiration() {
    return m_safetyHelper.getExpiration();
  }

  @Override
  public boolean isAlive() {
    return m_safetyHelper.isAlive();
  }

  @Override
  public boolean isSafetyEnabled() {
    return m_safetyHelper.isSafetyEnabled();
  }

  @Override
  public void setSafetyEnabled(boolean enabled) {
    m_safetyHelper.setSafetyEnabled(enabled);
  }

  @Override
  public String getDescription() {
    return "CANTalon ID " + m_deviceNumber;
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */

  private ITable m_table = null;
  private ITableListener m_table_listener = null;

  /**
   * {@inheritDoc}
   */
  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void updateTable() {
    CANSpeedController.super.updateTable();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public ITable getTable() {
    return m_table;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void startLiveWindowMode() {
    set(0); // Stop for safety
    m_table_listener = createTableListener();
    m_table.addTableListener(m_table_listener, true);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void stopLiveWindowMode() {
    set(0); // Stop for safety
    // TODO: See if this is still broken
    m_table.removeTableListener(m_table_listener);
  }

}
