/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.wpilibj.can.CANJNI;
import edu.wpi.first.wpilibj.can.CANMessageNotFoundException;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import edu.wpi.first.wpilibj.util.AllocationException;
import edu.wpi.first.wpilibj.util.CheckedAllocationException;

/**
 * Texas Instruments Jaguar Speed Controller as a CAN device.
 *$
 * @author Thomas Clark
 */
public class CANJaguar implements MotorSafety, PIDOutput, CANSpeedController {

  public static final int kMaxMessageDataSize = 8;

  // The internal PID control loop in the Jaguar runs at 1kHz.
  public static final int kControllerRate = 1000;
  public static final double kApproxBusVoltage = 12.0;

  private MotorSafetyHelper m_safetyHelper;
  private static final Resource allocated = new Resource(63);

  private static final int kFullMessageIDMask = CANJNI.CAN_MSGID_API_M | CANJNI.CAN_MSGID_MFR_M
      | CANJNI.CAN_MSGID_DTYPE_M;
  private static final int kSendMessagePeriod = 20;

  // Control Mode tags
  private static class EncoderTag {
  };

  /**
   * Sets an encoder as the speed reference only. <br>
   * Passed as the "tag" when setting the control mode.
   */
  public final static EncoderTag kEncoder = new EncoderTag();

  private static class QuadEncoderTag {
  };

  /**
   * Sets a quadrature encoder as the position and speed reference. <br>
   * Passed as the "tag" when setting the control mode.
   */
  public final static QuadEncoderTag kQuadEncoder = new QuadEncoderTag();

  private static class PotentiometerTag {
  };

  /**
   * Sets a potentiometer as the position reference only. <br>
   * Passed as the "tag" when setting the control mode.
   */
  public final static PotentiometerTag kPotentiometer = new PotentiometerTag();

  private boolean isInverted = false;

  /**
   * Mode determines how the Jaguar is controlled, used internally.
   */
  public enum JaguarControlMode implements CANSpeedController.ControlMode {
    PercentVbus, Current, Speed, Position, Voltage;

    @Override
    public boolean isPID() {
        return this == Current || this == Speed || this == Position;
    }

    @Override
    public int getValue() {
        return ordinal();
    }

  }

  public static final int kCurrentFault = 1;
  public static final int kTemperatureFault = 2;
  public static final int kBusVoltageFault = 4;
  public static final int kGateDriverFault = 8;

  /**
   * Limit switch masks
   */
  public static final int kForwardLimit = 1;
  public static final int kReverseLimit = 2;

  /**
   * Determines how the Jaguar behaves when sending a zero signal.
   */
  public enum NeutralMode {
    /** Use the NeutralMode that is set by the jumper wire on the CAN device */
    Jumper((byte) 0),
    /** Stop the motor's rotation by applying a force. */
    Brake((byte) 1),
    /**
     * Do not attempt to stop the motor. Instead allow it to coast to a stop
     * without applying resistance.
     */
    Coast((byte) 2);

    public byte value;

    public static NeutralMode valueOf(byte value) {
      for (NeutralMode mode : values()) {
        if (mode.value == value) {
          return mode;
        }
      }

      return null;
    }

    private NeutralMode(byte value) {
      this.value = value;
    }
  }


  /**
   * Determines which sensor to use for position reference. Limit switches will
   * always be used to limit the rotation. This can not be disabled.
   */
  public enum LimitMode {
    /**
     * Disables the soft position limits and only uses the limit switches to
     * limit rotation.
     *$
     * @see CANJaguar#getForwardLimitOK()
     * @see CANJaguar#getReverseLimitOK()
     */
    SwitchInputsOnly((byte) 0),
    /**
     * Enables the soft position limits on the Jaguar. These will be used in
     * addition to the limit switches. This does not disable the behavior of the
     * limit switch input.
     *$
     * @see CANJaguar#configSoftPositionLimits(double, double)
     */
    SoftPositionLimits((byte) 1);

    public byte value;

    public static LimitMode valueOf(byte value) {
      for (LimitMode mode : values()) {
        if (mode.value == value) {
          return mode;
        }
      }

      return null;
    }

    private LimitMode(byte value) {
      this.value = value;
    }
  }

  /**
   * Constructor for the CANJaguar device.<br>
   * By default the device is configured in Percent mode. The control mode can
   * be changed by calling one of the control modes listed below.
   *
   * @param deviceNumber The address of the Jaguar on the CAN bus.
   * @see CANJaguar#setCurrentMode(double, double, double)
   * @see CANJaguar#setCurrentMode(PotentiometerTag, double, double, double)
   * @see CANJaguar#setCurrentMode(EncoderTag, int, double, double, double)
   * @see CANJaguar#setCurrentMode(QuadEncoderTag, int, double, double, double)
   * @see CANJaguar#setPercentMode()
   * @see CANJaguar#setPercentMode(PotentiometerTag)
   * @see CANJaguar#setPercentMode(EncoderTag, int)
   * @see CANJaguar#setPercentMode(QuadEncoderTag, int)
   * @see CANJaguar#setPositionMode(PotentiometerTag, double, double, double)
   * @see CANJaguar#setPositionMode(QuadEncoderTag, int, double, double, double)
   * @see CANJaguar#setSpeedMode(EncoderTag, int, double, double, double)
   * @see CANJaguar#setSpeedMode(QuadEncoderTag, int, double, double, double)
   * @see CANJaguar#setVoltageMode()
   * @see CANJaguar#setVoltageMode(PotentiometerTag)
   * @see CANJaguar#setVoltageMode(EncoderTag, int)
   * @see CANJaguar#setVoltageMode(QuadEncoderTag, int)
   */
  public CANJaguar(int deviceNumber) {
    try {
      allocated.allocate(deviceNumber - 1);
    } catch (CheckedAllocationException e1) {
      throw new AllocationException("CANJaguar device " + e1.getMessage()
          + "(increment index by one)");
    }

    m_deviceNumber = (byte) deviceNumber;
    m_controlMode = JaguarControlMode.PercentVbus;

    m_safetyHelper = new MotorSafetyHelper(this);

    boolean receivedFirmwareVersion = false;
    byte[] data = new byte[8];

    // Request firmware and hardware version only once
    requestMessage(CANJNI.CAN_IS_FRAME_REMOTE | CANJNI.CAN_MSGID_API_FIRMVER);
    requestMessage(CANJNI.LM_API_HWVER);

    // Wait until we've gotten all of the status data at least once.
    for (int i = 0; i < kReceiveStatusAttempts; i++) {
      Timer.delay(0.001);

      setupPeriodicStatus();
      updatePeriodicStatus();

      if (!receivedFirmwareVersion) {
        try {
          getMessage(CANJNI.CAN_MSGID_API_FIRMVER, CANJNI.CAN_MSGID_FULL_M, data);
          m_firmwareVersion = unpackINT32(data);
          receivedFirmwareVersion = true;
        } catch (CANMessageNotFoundException e) {
        }
      }

      if (m_receivedStatusMessage0 && m_receivedStatusMessage1 && m_receivedStatusMessage2
          && receivedFirmwareVersion) {
        break;
      }
    }

    if (!m_receivedStatusMessage0 || !m_receivedStatusMessage1 || !m_receivedStatusMessage2
        || !receivedFirmwareVersion) {
      /* Free the resource */
      free();
      throw new CANMessageNotFoundException();
    }

    try {
      getMessage(CANJNI.LM_API_HWVER, CANJNI.CAN_MSGID_FULL_M, data);
      m_hardwareVersion = data[0];
    } catch (CANMessageNotFoundException e) {
      // Not all Jaguar firmware reports a hardware version.
      m_hardwareVersion = 0;
    }

    // 3330 was the first shipping RDK firmware version for the Jaguar
    if (m_firmwareVersion >= 3330 || m_firmwareVersion < 108) {
      if (m_firmwareVersion < 3330) {
        DriverStation.reportError("Jag " + m_deviceNumber + " firmware " + m_firmwareVersion
            + " is too old (must be at least version 108 of the FIRST approved firmware)", false);
      } else {
        DriverStation
            .reportError(
                "Jag"
                    + m_deviceNumber
                    + " firmware "
                    + m_firmwareVersion
                    + " is not FIRST approved (must be at least version 108 of the FIRST approved firmware)",
                false);
      }
      return;
    }
  }

  /**
   * Cancel periodic messages to the Jaguar, effectively disabling it. No other
   * methods should be called after this is called.
   */
  public void free() {
    allocated.free(m_deviceNumber - 1);
    m_safetyHelper = null;

    int messageID;

    // Disable periodic setpoints
    switch (m_controlMode) {
      case PercentVbus:
        messageID = m_deviceNumber | CANJNI.LM_API_VOLT_T_SET;
        break;

      case Speed:
        messageID = m_deviceNumber | CANJNI.LM_API_SPD_T_SET;
        break;

      case Position:
        messageID = m_deviceNumber | CANJNI.LM_API_POS_T_SET;
        break;

      case Current:
        messageID = m_deviceNumber | CANJNI.LM_API_ICTRL_T_SET;
        break;

      case Voltage:
        messageID = m_deviceNumber | CANJNI.LM_API_VCOMP_T_SET;
        break;

      default:
        return;
    }

    CANJNI.FRCNetworkCommunicationCANSessionMuxSendMessage(messageID, null,
        CANJNI.CAN_SEND_PERIOD_STOP_REPEATING);

    configMaxOutputVoltage(kApproxBusVoltage);
  }

  /**
   * @return The CAN ID passed in the constructor
   */
  int getDeviceNumber() {
    return m_deviceNumber;
  }

  /**
   * Get the recently set outputValue set point.
   *
   * The scale and the units depend on the mode the Jaguar is in.<br>
   * In percentVbus mode, the outputValue is from -1.0 to 1.0 (same as PWM
   * Jaguar).<br>
   * In voltage mode, the outputValue is in volts.<br>
   * In current mode, the outputValue is in amps.<br>
   * In speed mode, the outputValue is in rotations/minute.<br>
   * In position mode, the outputValue is in rotations.<br>
   *
   * @return The most recently set outputValue set point.
   */
  @Override
  public double get() {
    return m_value;
  }

  /**
   * Equivalent to {@link #get()}.
   */
  @Override
  public double getSetpoint() {
    return get();
  }

  /**
   * Get the difference between the setpoint and goal in closed loop modes.
   *
   * Outside of position and velocity modes the return value of getError() has
   * relatively little meaning.
   *
   * @return The difference between the setpoint and the current position.
   */
  @Override
  public double getError() {
    return get() - getPosition();
  }

  /**
   * Sets the output set-point value.
   *
   * The scale and the units depend on the mode the Jaguar is in.<br>
   * In percentVbus Mode, the outputValue is from -1.0 to 1.0 (same as PWM
   * Jaguar).<br>
   * In voltage Mode, the outputValue is in volts. <br>
   * In current Mode, the outputValue is in amps.<br>
   * In speed mode, the outputValue is in rotations/minute.<br>
   * In position Mode, the outputValue is in rotations.
   *
   * @param outputValue The set-point to sent to the motor controller.
   * @param syncGroup The update group to add this set() to, pending
   *        UpdateSyncGroup(). If 0, update immediately.
   */
  @Override
  public void set(double outputValue, byte syncGroup) {
    int messageID;
    byte[] data = new byte[8];
    byte dataSize;

    if (m_safetyHelper != null)
      m_safetyHelper.feed();

    if (m_stopped) {
      enableControl();
      m_stopped = false;
    }

    if (m_controlEnabled) {
      switch (m_controlMode) {
        case PercentVbus:
          messageID = CANJNI.LM_API_VOLT_T_SET;
          dataSize = packPercentage(data, isInverted ? -outputValue : outputValue);
          break;

        case Speed:
          messageID = CANJNI.LM_API_SPD_T_SET;
          dataSize = packFXP16_16(data, isInverted ? -outputValue : outputValue);
          break;

        case Position:
          messageID = CANJNI.LM_API_POS_T_SET;
          dataSize = packFXP16_16(data, outputValue);
          break;

        case Current:
          messageID = CANJNI.LM_API_ICTRL_T_SET;
          dataSize = packFXP8_8(data, outputValue);
          break;


        case Voltage:
          messageID = CANJNI.LM_API_VCOMP_T_SET;
          dataSize = packFXP8_8(data, isInverted ? -outputValue : outputValue);
          break;

        default:
          return;
      }

      if (syncGroup != 0) {
        data[dataSize++] = syncGroup;
      }

      sendMessage(messageID, data, dataSize, kSendMessagePeriod);
    }

    m_value = outputValue;

    verify();
  }

  /**
   * Sets the output set-point value.
   *
   * The scale and the units depend on the mode the Jaguar is in.<br>
   * In percentVbus mode, the outputValue is from -1.0 to 1.0 (same as PWM
   * Jaguar).<br>
   * In voltage mode, the outputValue is in volts. <br>
   * In current mode, the outputValue is in amps. <br>
   * In speed mode, the outputValue is in rotations/minute.<br>
   * In position mode, the outputValue is in rotations.
   *
   * @param value The set-point to sent to the motor controller.
   */
  @Override
  public void set(double value) {
    set(value, (byte) 0);
  }

  /**
   * Equivalent to {@link #set(double)}. Implements PIDInterface.
   */
  @Override
  public void setSetpoint(double value) {
    set(value);
  }

  @Override
  public void reset() {
    set(m_value);
    disableControl();
  }

  /**
   * Inverts the direction of rotation of the motor Only works in percentVbus,
   * Speed, and Voltage mode
   *$
   * @param isInverted The state of inversion true is inverted
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
   * Check all unverified params and make sure they're equal to their local
   * cached versions. If a value isn't available, it gets requested. If a value
   * doesn't match up, it gets set again.
   */
  protected void verify() {
    byte[] data = new byte[8];

    // If the Jaguar lost power, everything should be considered unverified
    try {
      getMessage(CANJNI.LM_API_STATUS_POWER, CANJNI.CAN_MSGID_FULL_M, data);
      boolean powerCycled = data[0] != 0;

      if (powerCycled) {
        // Clear the power cycled bit
        data[0] = 1;
        sendMessage(CANJNI.LM_API_STATUS_POWER, data, 1);

        // Mark everything as unverified
        m_controlModeVerified = false;
        m_speedRefVerified = false;
        m_posRefVerified = false;
        m_neutralModeVerified = false;
        m_encoderCodesPerRevVerified = false;
        m_potentiometerTurnsVerified = false;
        m_forwardLimitVerified = false;
        m_reverseLimitVerified = false;
        m_limitModeVerified = false;
        m_maxOutputVoltageVerified = false;
        m_faultTimeVerified = false;

        if (m_controlMode == JaguarControlMode.PercentVbus || m_controlMode == JaguarControlMode.Voltage) {
          m_voltageRampRateVerified = false;
        } else {
          m_pVerified = false;
          m_iVerified = false;
          m_dVerified = false;
        }

        // Verify periodic status messages again
        m_receivedStatusMessage0 = false;
        m_receivedStatusMessage1 = false;
        m_receivedStatusMessage2 = false;

        // Remove any old values from netcomms. Otherwise, parameters
        // are incorrectly marked as verified based on stale messages.
        int[] messages =
            new int[] {CANJNI.LM_API_SPD_REF, CANJNI.LM_API_POS_REF, CANJNI.LM_API_SPD_PC,
                CANJNI.LM_API_POS_PC, CANJNI.LM_API_ICTRL_PC, CANJNI.LM_API_SPD_IC,
                CANJNI.LM_API_POS_IC, CANJNI.LM_API_ICTRL_IC, CANJNI.LM_API_SPD_DC,
                CANJNI.LM_API_POS_DC, CANJNI.LM_API_ICTRL_DC, CANJNI.LM_API_CFG_ENC_LINES,
                CANJNI.LM_API_CFG_POT_TURNS, CANJNI.LM_API_CFG_BRAKE_COAST,
                CANJNI.LM_API_CFG_LIMIT_MODE, CANJNI.LM_API_CFG_LIMIT_REV,
                CANJNI.LM_API_CFG_MAX_VOUT, CANJNI.LM_API_VOLT_SET_RAMP,
                CANJNI.LM_API_VCOMP_COMP_RAMP, CANJNI.LM_API_CFG_FAULT_TIME,
                CANJNI.LM_API_CFG_LIMIT_FWD};

        for (int message : messages) {
          try {
            getMessage(message, CANJNI.CAN_MSGID_FULL_M, data);
          } catch (CANMessageNotFoundException e) {
          }
        }
      }
    } catch (CANMessageNotFoundException e) {
      requestMessage(CANJNI.LM_API_STATUS_POWER);
    }

    // Verify that any recently set parameters are correct
    if (!m_controlModeVerified && m_controlEnabled) {
      try {
        getMessage(CANJNI.LM_API_STATUS_CMODE, CANJNI.CAN_MSGID_FULL_M, data);

        JaguarControlMode mode = JaguarControlMode.values()[data[0]];

        if (m_controlMode == mode) {
          m_controlModeVerified = true;
        } else {
          // Enable control again to resend the control mode
          enableControl();
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_STATUS_CMODE);
      }
    }

    if (!m_speedRefVerified) {
      try {
        getMessage(CANJNI.LM_API_SPD_REF, CANJNI.CAN_MSGID_FULL_M, data);

        int speedRef = data[0];

        if (m_speedReference == speedRef) {
          m_speedRefVerified = true;
        } else {
          // It's wrong - set it again
          setSpeedReference(m_speedReference);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_SPD_REF);
      }
    }

    if (!m_posRefVerified) {
      try {
        getMessage(CANJNI.LM_API_POS_REF, CANJNI.CAN_MSGID_FULL_M, data);

        int posRef = data[0];

        if (m_positionReference == posRef) {
          m_posRefVerified = true;
        } else {
          // It's wrong - set it again
          setPositionReference(m_positionReference);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_POS_REF);
      }
    }

    if (!m_pVerified) {
      int message = 0;

      switch (m_controlMode) {
        case Speed:
          message = CANJNI.LM_API_SPD_PC;
          break;

        case Position:
          message = CANJNI.LM_API_POS_PC;
          break;

        case Current:
          message = CANJNI.LM_API_ICTRL_PC;
          break;

        default:
          break;
      }

      try {
        getMessage(message, CANJNI.CAN_MSGID_FULL_M, data);

        double p = unpackFXP16_16(data);

        if (FXP16_EQ(m_p, p)) {
          m_pVerified = true;
        } else {
          // It's wrong - set it again
          setP(m_p);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(message);
      }
    }

    if (!m_iVerified) {
      int message = 0;

      switch (m_controlMode) {
        case Speed:
          message = CANJNI.LM_API_SPD_IC;
          break;

        case Position:
          message = CANJNI.LM_API_POS_IC;
          break;

        case Current:
          message = CANJNI.LM_API_ICTRL_IC;
          break;

        default:
          break;
      }

      try {
        getMessage(message, CANJNI.CAN_MSGID_FULL_M, data);

        double i = unpackFXP16_16(data);

        if (FXP16_EQ(m_i, i)) {
          m_iVerified = true;
        } else {
          // It's wrong - set it again
          setI(m_i);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(message);
      }
    }

    if (!m_dVerified) {
      int message = 0;

      switch (m_controlMode) {
        case Speed:
          message = CANJNI.LM_API_SPD_DC;
          break;

        case Position:
          message = CANJNI.LM_API_POS_DC;
          break;

        case Current:
          message = CANJNI.LM_API_ICTRL_DC;
          break;

        default:
          break;
      }

      try {
        getMessage(message, CANJNI.CAN_MSGID_FULL_M, data);

        double d = unpackFXP16_16(data);

        if (FXP16_EQ(m_d, d)) {
          m_dVerified = true;
        } else {
          // It's wrong - set it again
          setD(m_d);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(message);
      }
    }

    if (!m_neutralModeVerified) {
      try {
        getMessage(CANJNI.LM_API_CFG_BRAKE_COAST, CANJNI.CAN_MSGID_FULL_M, data);

        NeutralMode mode = NeutralMode.valueOf(data[0]);

        if (mode == m_neutralMode) {
          m_neutralModeVerified = true;
        } else {
          // It's wrong - set it again
          configNeutralMode(m_neutralMode);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_CFG_BRAKE_COAST);
      }
    }

    if (!m_encoderCodesPerRevVerified) {
      try {
        getMessage(CANJNI.LM_API_CFG_ENC_LINES, CANJNI.CAN_MSGID_FULL_M, data);

        short codes = unpackINT16(data);

        if (codes == m_encoderCodesPerRev) {
          m_encoderCodesPerRevVerified = true;
        } else {
          // It's wrong - set it again
          configEncoderCodesPerRev(m_encoderCodesPerRev);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_CFG_ENC_LINES);
      }
    }

    if (!m_potentiometerTurnsVerified) {
      try {
        getMessage(CANJNI.LM_API_CFG_POT_TURNS, CANJNI.CAN_MSGID_FULL_M, data);

        short turns = unpackINT16(data);

        if (turns == m_potentiometerTurns) {
          m_potentiometerTurnsVerified = true;
        } else {
          // It's wrong - set it again
          configPotentiometerTurns(m_potentiometerTurns);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_CFG_POT_TURNS);
      }
    }

    if (!m_limitModeVerified) {
      try {
        getMessage(CANJNI.LM_API_CFG_LIMIT_MODE, CANJNI.CAN_MSGID_FULL_M, data);

        LimitMode mode = LimitMode.valueOf(data[0]);

        if (mode == m_limitMode) {
          m_limitModeVerified = true;
        } else {
          // It's wrong - set it again
          configLimitMode(m_limitMode);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_CFG_LIMIT_MODE);
      }
    }

    if (!m_forwardLimitVerified) {
      try {
        getMessage(CANJNI.LM_API_CFG_LIMIT_FWD, CANJNI.CAN_MSGID_FULL_M, data);

        double limit = unpackFXP16_16(data);

        if (FXP16_EQ(limit, m_forwardLimit)) {
          m_forwardLimitVerified = true;
        } else {
          // It's wrong - set it again
          configForwardLimit(m_forwardLimit);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_CFG_LIMIT_FWD);
      }
    }

    if (!m_reverseLimitVerified) {
      try {
        getMessage(CANJNI.LM_API_CFG_LIMIT_REV, CANJNI.CAN_MSGID_FULL_M, data);

        double limit = unpackFXP16_16(data);

        if (FXP16_EQ(limit, m_reverseLimit)) {
          m_reverseLimitVerified = true;
        } else {
          // It's wrong - set it again
          configReverseLimit(m_reverseLimit);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_CFG_LIMIT_REV);
      }
    }

    if (!m_maxOutputVoltageVerified) {
      try {
        getMessage(CANJNI.LM_API_CFG_MAX_VOUT, CANJNI.CAN_MSGID_FULL_M, data);

        double voltage = unpackFXP8_8(data);

        // The returned max output voltage is sometimes slightly higher
        // or lower than what was sent. This should not trigger
        // resending the message.
        if (Math.abs(voltage - m_maxOutputVoltage) < 0.1) {
          m_maxOutputVoltageVerified = true;
        } else {
          // It's wrong - set it again
          configMaxOutputVoltage(m_maxOutputVoltage);
        }

      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_CFG_MAX_VOUT);
      }
    }

    if (!m_voltageRampRateVerified) {
      if (m_controlMode == JaguarControlMode.PercentVbus) {
        try {
          getMessage(CANJNI.LM_API_VOLT_SET_RAMP, CANJNI.CAN_MSGID_FULL_M, data);

          double rate = unpackPercentage(data);

          if (FXP16_EQ(rate, m_voltageRampRate)) {
            m_voltageRampRateVerified = true;
          } else {
            // It's wrong - set it again
            setVoltageRampRate(m_voltageRampRate);
          }

        } catch (CANMessageNotFoundException e) {
          // Verification is needed but not available - request it again.
          requestMessage(CANJNI.LM_API_VOLT_SET_RAMP);
        }
      }
    } else if (m_controlMode == JaguarControlMode.Voltage) {
      try {
        getMessage(CANJNI.LM_API_VCOMP_COMP_RAMP, CANJNI.CAN_MSGID_FULL_M, data);

        double rate = unpackFXP8_8(data);

        if (FXP8_EQ(rate, m_voltageRampRate)) {
          m_voltageRampRateVerified = true;
        } else {
          // It's wrong - set it again
          setVoltageRampRate(m_voltageRampRate);
        }

      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_VCOMP_COMP_RAMP);
      }
    }

    if (!m_faultTimeVerified) {
      try {
        getMessage(CANJNI.LM_API_CFG_FAULT_TIME, CANJNI.CAN_MSGID_FULL_M, data);

        int faultTime = unpackINT16(data);

        if ((int) (m_faultTime * 1000.0) == faultTime) {
          m_faultTimeVerified = true;
        } else {
          // It's wrong - set it again
          configFaultTime(m_faultTime);
        }
      } catch (CANMessageNotFoundException e) {
        // Verification is needed but not available - request it again.
        requestMessage(CANJNI.LM_API_CFG_FAULT_TIME);
      }
    }

    if (!m_receivedStatusMessage0 || !m_receivedStatusMessage1 || !m_receivedStatusMessage2) {
      // If the periodic status messages haven't been verified as received,
      // request periodic status messages again and attempt to unpack any
      // available ones.
      setupPeriodicStatus();
      getTemperature();
      getPosition();
      getFaults();
    }
  }

  /**
   * Common interface for disabling a motor.
   *
   * @deprecated Call {@link #disableControl()} instead.
   */
  @Deprecated
  @Override
  public void disable() {
    disableControl();
  }

  // PIDInterface interface
  public void enable() {
    enableControl();
  }

  // PIDOutput interface
  @Override
  public void pidWrite(double output) {
    if (m_controlMode == JaguarControlMode.PercentVbus) {
      set(output);
    } else {
      throw new IllegalStateException("PID only supported in PercentVbus mode");
    }
  }

  /**
   * Set the reference source device for speed controller mode.
   *
   * Choose encoder as the source of speed feedback when in speed control mode.
   *
   * @param reference Specify a speed reference.
   */
  private void setSpeedReference(int reference) {
    sendMessage(CANJNI.LM_API_SPD_REF, new byte[] {(byte) reference}, 1);

    m_speedReference = reference;
    m_speedRefVerified = false;
  }

  /**
   * Set the reference source device for position controller mode.
   *
   * Choose between using and encoder and using a potentiometer as the source of
   * position feedback when in position control mode.
   *
   * @param reference Specify a position reference.
   */
  private void setPositionReference(int reference) {
    sendMessage(CANJNI.LM_API_POS_REF, new byte[] {(byte) reference}, 1);

    m_positionReference = reference;
    m_posRefVerified = false;
  }

  /**
   * Set the P constant for the closed loop modes.
   *
   * @param p The proportional gain of the Jaguar's PID controller.
   */
  public void setP(double p) {
    byte[] data = new byte[8];
    byte dataSize = packFXP16_16(data, p);

    switch (m_controlMode) {
      case Speed:
        sendMessage(CANJNI.LM_API_SPD_PC, data, dataSize);
        break;

      case Position:
        sendMessage(CANJNI.LM_API_POS_PC, data, dataSize);
        break;

      case Current:
        sendMessage(CANJNI.LM_API_ICTRL_PC, data, dataSize);
        break;

      default:
        throw new IllegalStateException(
            "PID constants only apply in Speed, Position, and Current mode");
    }

    m_p = p;
    m_pVerified = false;
  }

  /**
   * Set the I constant for the closed loop modes.
   *
   * @param i The integral gain of the Jaguar's PID controller.
   */
  public void setI(double i) {
    byte[] data = new byte[8];
    byte dataSize = packFXP16_16(data, i);

    switch (m_controlMode) {
      case Speed:
        sendMessage(CANJNI.LM_API_SPD_IC, data, dataSize);
        break;

      case Position:
        sendMessage(CANJNI.LM_API_POS_IC, data, dataSize);
        break;

      case Current:
        sendMessage(CANJNI.LM_API_ICTRL_IC, data, dataSize);
        break;

      default:
        throw new IllegalStateException(
            "PID constants only apply in Speed, Position, and Current mode");
    }

    m_i = i;
    m_iVerified = false;
  }

  /**
   * Set the D constant for the closed loop modes.
   *
   * @param d The derivative gain of the Jaguar's PID controller.
   */
  public void setD(double d) {
    byte[] data = new byte[8];
    byte dataSize = packFXP16_16(data, d);

    switch (m_controlMode) {
      case Speed:
        sendMessage(CANJNI.LM_API_SPD_DC, data, dataSize);
        break;

      case Position:
        sendMessage(CANJNI.LM_API_POS_DC, data, dataSize);
        break;

      case Current:
        sendMessage(CANJNI.LM_API_ICTRL_DC, data, dataSize);
        break;

      default:
        throw new IllegalStateException(
            "PID constants only apply in Speed, Position, and Current mode");
    }

    m_d = d;
    m_dVerified = false;
  }

  /**
   * Set the P, I, and D constants for the closed loop modes.
   *
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   */
  public void setPID(double p, double i, double d) {
    setP(p);
    setI(i);
    setD(d);
  }

  /**
   * Get the Proportional gain of the controller.
   *
   * @return The proportional gain.
   */
  public double getP() {
    if (m_controlMode.equals(JaguarControlMode.PercentVbus) || m_controlMode.equals(JaguarControlMode.Voltage)) {
      throw new IllegalStateException("PID does not apply in Percent or Voltage control modes");
    }
    return m_p;
  }

  /**
   * Get the Integral gain of the controller.
   *
   * @return The integral gain.
   */
  public double getI() {
    if (m_controlMode.equals(JaguarControlMode.PercentVbus) || m_controlMode.equals(JaguarControlMode.Voltage)) {
      throw new IllegalStateException("PID does not apply in Percent or Voltage control modes");
    }
    return m_i;
  }

  /**
   * Get the Derivative gain of the controller.
   *
   * @return The derivative gain.
   */
  public double getD() {
    if (m_controlMode.equals(JaguarControlMode.PercentVbus) || m_controlMode.equals(JaguarControlMode.Voltage)) {
      throw new IllegalStateException("PID does not apply in Percent or Voltage control modes");
    }
    return m_d;
  }

  /**
   * Enable the closed loop controller.
   *
   * Start actually controlling the output based on the feedback. If starting a
   * position controller with an encoder reference, use the
   * encoderInitialPosition parameter to initialize the encoder state.
   *
   * @param encoderInitialPosition Encoder position to set if position with
   *        encoder reference. Ignored otherwise.
   */
  public void enableControl(double encoderInitialPosition) {
    switch (m_controlMode) {
      case PercentVbus:
        sendMessage(CANJNI.LM_API_VOLT_T_EN, new byte[0], 0);
        break;

      case Speed:
        sendMessage(CANJNI.LM_API_SPD_T_EN, new byte[0], 0);
        break;

      case Position:
        byte[] data = new byte[8];
        int dataSize = packFXP16_16(data, encoderInitialPosition);
        sendMessage(CANJNI.LM_API_POS_T_EN, data, dataSize);
        break;

      case Current:
        sendMessage(CANJNI.LM_API_ICTRL_T_EN, new byte[0], 0);
        break;

      case Voltage:
        sendMessage(CANJNI.LM_API_VCOMP_T_EN, new byte[0], 0);
        break;
    }

    m_controlEnabled = true;
  }

  /**
   * Enable the closed loop controller.
   *
   * Start actually controlling the output based on the feedback. This is the
   * same as calling
   * <code>CANJaguar.enableControl(double encoderInitialPosition)</code> with
   * <code>encoderInitialPosition</code> set to <code>0.0</code>
   */
  public void enableControl() {
    enableControl(0.0);
  }

  /**
   * Return whether the controller is enabled.
   *
   * @return true if enabled.
   */
  public boolean isEnabled() {
    return m_controlEnabled;
  }

  /**
   * Disable the closed loop controller.
   *
   * Stop driving the output based on the feedback.
   */
  public void disableControl() {
    // Disable all control modes.
    sendMessage(CANJNI.LM_API_VOLT_DIS, new byte[0], 0);
    sendMessage(CANJNI.LM_API_SPD_DIS, new byte[0], 0);
    sendMessage(CANJNI.LM_API_POS_DIS, new byte[0], 0);
    sendMessage(CANJNI.LM_API_ICTRL_DIS, new byte[0], 0);
    sendMessage(CANJNI.LM_API_VCOMP_DIS, new byte[0], 0);

    // Stop all periodic setpoints
    sendMessage(CANJNI.LM_API_VOLT_T_SET, new byte[0], 0, CANJNI.CAN_SEND_PERIOD_STOP_REPEATING);
    sendMessage(CANJNI.LM_API_SPD_T_SET, new byte[0], 0, CANJNI.CAN_SEND_PERIOD_STOP_REPEATING);
    sendMessage(CANJNI.LM_API_POS_T_SET, new byte[0], 0, CANJNI.CAN_SEND_PERIOD_STOP_REPEATING);
    sendMessage(CANJNI.LM_API_ICTRL_T_SET, new byte[0], 0, CANJNI.CAN_SEND_PERIOD_STOP_REPEATING);
    sendMessage(CANJNI.LM_API_VCOMP_T_SET, new byte[0], 0, CANJNI.CAN_SEND_PERIOD_STOP_REPEATING);

    m_controlEnabled = false;
  }

  /**
   * Enable controlling the motor voltage as a percentage of the bus voltage
   * without any position or speed feedback.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   */
  public void setPercentMode() {
    changeControlMode(JaguarControlMode.PercentVbus);
    setPositionReference(CANJNI.LM_REF_NONE);
    setSpeedReference(CANJNI.LM_REF_NONE);
  }

  /**
   * Enable controlling the motor voltage as a percentage of the bus voltage,
   * and enable speed sensing from a non-quadrature encoder.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kEncoder}
   * @param codesPerRev The counts per revolution on the encoder
   */
  public void setPercentMode(EncoderTag tag, int codesPerRev) {
    changeControlMode(JaguarControlMode.PercentVbus);
    setPositionReference(CANJNI.LM_REF_NONE);
    setSpeedReference(CANJNI.LM_REF_ENCODER);
    configEncoderCodesPerRev(codesPerRev);
  }

  /**
   * Enable controlling the motor voltage as a percentage of the bus voltage,
   * and enable position and speed sensing from a quadrature encoder.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kQuadEncoder}
   * @param codesPerRev The counts per revolution on the encoder
   */
  public void setPercentMode(QuadEncoderTag tag, int codesPerRev) {
    changeControlMode(JaguarControlMode.PercentVbus);
    setPositionReference(CANJNI.LM_REF_ENCODER);
    setSpeedReference(CANJNI.LM_REF_QUAD_ENCODER);
    configEncoderCodesPerRev(codesPerRev);
  }

  /**
   * Enable controlling the motor voltage as a percentage of the bus voltage,
   * and enable position sensing from a potentiometer and no speed feedback.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kPotentiometer}
   */
  public void setPercentMode(PotentiometerTag tag) {
    changeControlMode(JaguarControlMode.PercentVbus);
    setPositionReference(CANJNI.LM_REF_POT);
    setSpeedReference(CANJNI.LM_REF_NONE);
    configPotentiometerTurns(1);
  }

  /**
   * Enable controlling the motor current with a PID loop.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   */
  public void setCurrentMode(double p, double i, double d) {
    changeControlMode(JaguarControlMode.Current);
    setPositionReference(CANJNI.LM_REF_NONE);
    setSpeedReference(CANJNI.LM_REF_NONE);
    setPID(p, i, d);
  }

  /**
   * Enable controlling the motor current with a PID loop, and enable speed
   * sensing from a non-quadrature encoder.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kEncoder}
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   */
  public void setCurrentMode(EncoderTag tag, int codesPerRev, double p, double i, double d) {
    changeControlMode(JaguarControlMode.Current);
    setPositionReference(CANJNI.LM_REF_NONE);
    setSpeedReference(CANJNI.LM_REF_NONE);
    configEncoderCodesPerRev(codesPerRev);
    setPID(p, i, d);
  }

  /**
   * Enable controlling the motor current with a PID loop, and enable speed and
   * position sensing from a quadrature encoder.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kQuadEncoder}
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   */
  public void setCurrentMode(QuadEncoderTag tag, int codesPerRev, double p, double i, double d) {
    changeControlMode(JaguarControlMode.Current);
    setPositionReference(CANJNI.LM_REF_ENCODER);
    setSpeedReference(CANJNI.LM_REF_QUAD_ENCODER);
    configEncoderCodesPerRev(codesPerRev);
    setPID(p, i, d);
  }

  /**
   * Enable controlling the motor current with a PID loop, and enable position
   * sensing from a potentiometer.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kPotentiometer}
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   */
  public void setCurrentMode(PotentiometerTag tag, double p, double i, double d) {
    changeControlMode(JaguarControlMode.Current);
    setPositionReference(CANJNI.LM_REF_POT);
    setSpeedReference(CANJNI.LM_REF_NONE);
    configPotentiometerTurns(1);
    setPID(p, i, d);
  }

  /**
   * Enable controlling the speed with a feedback loop from a non-quadrature
   * encoder.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kEncoder}
   * @param codesPerRev The counts per revolution on the encoder
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   */
  public void setSpeedMode(EncoderTag tag, int codesPerRev, double p, double i, double d) {
    changeControlMode(JaguarControlMode.Speed);
    setPositionReference(CANJNI.LM_REF_NONE);
    setSpeedReference(CANJNI.LM_REF_ENCODER);
    configEncoderCodesPerRev(codesPerRev);
    setPID(p, i, d);
  }

  /**
   * Enable controlling the speed with a feedback loop from a quadrature
   * encoder.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kQuadEncoder}
   * @param codesPerRev The counts per revolution on the encoder
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   */
  public void setSpeedMode(QuadEncoderTag tag, int codesPerRev, double p, double i, double d) {
    changeControlMode(JaguarControlMode.Speed);
    setPositionReference(CANJNI.LM_REF_ENCODER);
    setSpeedReference(CANJNI.LM_REF_QUAD_ENCODER);
    configEncoderCodesPerRev(codesPerRev);
    setPID(p, i, d);
  }

  /**
   * Enable controlling the position with a feedback loop using an encoder.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kQuadEncoder}
   * @param codesPerRev The counts per revolution on the encoder
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   *
   */
  public void setPositionMode(QuadEncoderTag tag, int codesPerRev, double p, double i, double d) {
    changeControlMode(JaguarControlMode.Position);
    setPositionReference(CANJNI.LM_REF_ENCODER);
    configEncoderCodesPerRev(codesPerRev);
    setPID(p, i, d);
  }

  /**
   * Enable controlling the position with a feedback loop using a potentiometer.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kPotentiometer}
   * @param p The proportional gain of the Jaguar's PID controller.
   * @param i The integral gain of the Jaguar's PID controller.
   * @param d The differential gain of the Jaguar's PID controller.
   */
  public void setPositionMode(PotentiometerTag tag, double p, double i, double d) {
    changeControlMode(JaguarControlMode.Position);
    setPositionReference(CANJNI.LM_REF_POT);
    configPotentiometerTurns(1);
    setPID(p, i, d);
  }

  /**
   * Enable controlling the motor voltage without any position or speed
   * feedback.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   */
  public void setVoltageMode() {
    changeControlMode(JaguarControlMode.Voltage);
    setPositionReference(CANJNI.LM_REF_NONE);
    setSpeedReference(CANJNI.LM_REF_NONE);
  }

  /**
   * Enable controlling the motor voltage with speed feedback from a
   * non-quadrature encoder and no position feedback.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kEncoder}
   * @param codesPerRev The counts per revolution on the encoder
   */
  public void setVoltageMode(EncoderTag tag, int codesPerRev) {
    changeControlMode(JaguarControlMode.Voltage);
    setPositionReference(CANJNI.LM_REF_NONE);
    setSpeedReference(CANJNI.LM_REF_ENCODER);
    configEncoderCodesPerRev(codesPerRev);
  }

  /**
   * Enable controlling the motor voltage with position and speed feedback from
   * a quadrature encoder.<br>
   * After calling this you must call {@link CANJaguar#enableControl()} or
   * {@link CANJaguar#enableControl(double)} to enable the device.
   *
   * @param tag The constant {@link CANJaguar#kQuadEncoder}
   * @param codesPerRev The counts per revolution on the encoder
   */
  public void setVoltageMode(QuadEncoderTag tag, int codesPerRev) {
    changeControlMode(JaguarControlMode.Voltage);
    setPositionReference(CANJNI.LM_REF_ENCODER);
    setSpeedReference(CANJNI.LM_REF_QUAD_ENCODER);
    configEncoderCodesPerRev(codesPerRev);
  }

  /**
   * Enable controlling the motor voltage with position feedback from a
   * potentiometer and no speed feedback.
   *
   * @param tag The constant {@link CANJaguar#kPotentiometer}
   */
  public void setVoltageMode(PotentiometerTag tag) {
    changeControlMode(JaguarControlMode.Voltage);
    setPositionReference(CANJNI.LM_REF_POT);
    setSpeedReference(CANJNI.LM_REF_NONE);
    configPotentiometerTurns(1);
  }

  /**
   * Used internally. In order to set the control mode see the methods listed
   * below.
   *
   * Change the control mode of this Jaguar object.
   *
   * After changing modes, configure any PID constants or other settings needed
   * and then EnableControl() to actually change the mode on the Jaguar.
   *
   * @param controlMode The new mode.
   *
   * @see CANJaguar#setCurrentMode(double, double, double)
   * @see CANJaguar#setCurrentMode(PotentiometerTag, double, double, double)
   * @see CANJaguar#setCurrentMode(EncoderTag, int, double, double, double)
   * @see CANJaguar#setCurrentMode(QuadEncoderTag, int, double, double, double)
   * @see CANJaguar#setPercentMode()
   * @see CANJaguar#setPercentMode(PotentiometerTag)
   * @see CANJaguar#setPercentMode(EncoderTag, int)
   * @see CANJaguar#setPercentMode(QuadEncoderTag, int)
   * @see CANJaguar#setPositionMode(PotentiometerTag, double, double, double)
   * @see CANJaguar#setPositionMode(QuadEncoderTag, int, double, double, double)
   * @see CANJaguar#setSpeedMode(EncoderTag, int, double, double, double)
   * @see CANJaguar#setSpeedMode(QuadEncoderTag, int, double, double, double)
   * @see CANJaguar#setVoltageMode()
   * @see CANJaguar#setVoltageMode(PotentiometerTag)
   * @see CANJaguar#setVoltageMode(EncoderTag, int)
   * @see CANJaguar#setVoltageMode(QuadEncoderTag, int)
   */
  private void changeControlMode(JaguarControlMode controlMode) {
    // Disable the previous mode
    disableControl();

    // Update the local mode
    m_controlMode = controlMode;
    m_controlModeVerified = false;
  }

  /**
   * Get the active control mode from the Jaguar.
   *
   * Ask the Jagaur what mode it is in.
   *
   * @return JaguarControlMode that the Jag is in.
   */
  public JaguarControlMode getControlMode() {
    return m_controlMode;
  }

  public void setControlMode(int mode) {
      changeControlMode(JaguarControlMode.values()[mode]);
  }

  /**
   * Get the voltage at the battery input terminals of the Jaguar.
   *
   * @return The bus voltage in Volts.
   */
  public double getBusVoltage() {
    updatePeriodicStatus();

    return m_busVoltage;
  }

  /**
   * Get the voltage being output from the motor terminals of the Jaguar.
   *
   * @return The output voltage in Volts.
   */
  public double getOutputVoltage() {
    updatePeriodicStatus();

    return m_outputVoltage;
  }

  /**
   * Get the current through the motor terminals of the Jaguar.
   *
   * @return The output current in Amps.
   */
  public double getOutputCurrent() {
    updatePeriodicStatus();

    return m_outputCurrent;
  }

  /**
   * Get the internal temperature of the Jaguar.
   *
   * @return The temperature of the Jaguar in degrees Celsius.
   */
  public double getTemperature() {
    updatePeriodicStatus();

    return m_temperature;
  }

  /**
   * Get the position of the encoder or potentiometer.
   *
   * @return The position of the motor in rotations based on the configured
   *         feedback.
   * @see CANJaguar#configPotentiometerTurns(int)
   * @see CANJaguar#configEncoderCodesPerRev(int)
   */
  public double getPosition() {
    updatePeriodicStatus();

    return m_position;
  }

  /**
   * Get the speed of the encoder.
   *
   * @return The speed of the motor in RPM based on the configured feedback.
   */
  public double getSpeed() {
    updatePeriodicStatus();

    return m_speed;
  }

  /**
   * Get the status of the forward limit switch.
   *
   * @return true if the motor is allowed to turn in the forward direction.
   */
  public boolean getForwardLimitOK() {
    updatePeriodicStatus();

    return (m_limits & kForwardLimit) != 0;
  }

  /**
   * Get the status of the reverse limit switch.
   *
   * @return true if the motor is allowed to turn in the reverse direction.
   */
  public boolean getReverseLimitOK() {
    updatePeriodicStatus();

    return (m_limits & kReverseLimit) != 0;
  }

  /**
   * Get the status of any faults the Jaguar has detected.
   *
   * @return A bit-mask of faults defined by the "Faults" constants.
   * @see #kCurrentFault
   * @see #kBusVoltageFault
   * @see #kTemperatureFault
   * @see #kGateDriverFault
   */
  public short getFaults() {
    updatePeriodicStatus();

    return m_faults;
  }

  /**
   * set the maximum voltage change rate.
   *
   * When in PercentVbus or Voltage output mode, the rate at which the voltage
   * changes can be limited to reduce current spikes. set this to 0.0 to disable
   * rate limiting.
   *
   * @param rampRate The maximum rate of voltage change in Percent Voltage mode
   *        in V/s.
   */
  public void setVoltageRampRate(double rampRate) {
    byte[] data = new byte[8];
    int dataSize;
    int message;

    switch (m_controlMode) {
      case PercentVbus:
        dataSize = packPercentage(data, rampRate / (m_maxOutputVoltage * kControllerRate));
        message = CANJNI.LM_API_VOLT_SET_RAMP;
        break;
      case Voltage:
        dataSize = packFXP8_8(data, rampRate / kControllerRate);
        message = CANJNI.LM_API_VCOMP_COMP_RAMP;
        break;
      default:
        throw new IllegalStateException(
            "Voltage ramp rate only applies in Percentage and Voltage modes");
    }

    sendMessage(message, data, dataSize);
  }

  /**
   * Get the version of the firmware running on the Jaguar.
   *
   * @return The firmware version. 0 if the device did not respond.
   */
  public int getFirmwareVersion() {
    return m_firmwareVersion;
  }

  /**
   * Get the version of the Jaguar hardware.
   *
   * @return The hardware version. 1: Jaguar, 2: Black Jaguar
   */
  public byte getHardwareVersion() {
    return m_hardwareVersion;
  }

  /**
   * Configure what the controller does to the H-Bridge when neutral (not
   * driving the output).
   *
   * This allows you to override the jumper configuration for brake or coast.
   *
   * @param mode Select to use the jumper setting or to override it to coast or
   *        brake.
   */
  public void configNeutralMode(NeutralMode mode) {
    sendMessage(CANJNI.LM_API_CFG_BRAKE_COAST, new byte[] {mode.value}, 1);

    m_neutralMode = mode;
    m_neutralModeVerified = false;
  }

  /**
   * Configure how many codes per revolution are generated by your encoder.
   *
   * @param codesPerRev The number of counts per revolution in 1X mode.
   */
  public void configEncoderCodesPerRev(int codesPerRev) {
    byte[] data = new byte[8];

    int dataSize = packINT16(data, (short) codesPerRev);
    sendMessage(CANJNI.LM_API_CFG_ENC_LINES, data, dataSize);

    m_encoderCodesPerRev = (short) codesPerRev;
    m_encoderCodesPerRevVerified = false;
  }

  /**
   * Configure the number of turns on the potentiometer.
   *
   * There is no special support for continuous turn potentiometers. Only
   * integer numbers of turns are supported.
   *
   * @param turns The number of turns of the potentiometer
   */
  public void configPotentiometerTurns(int turns) {
    byte[] data = new byte[8];

    int dataSize = packINT16(data, (short) turns);
    sendMessage(CANJNI.LM_API_CFG_POT_TURNS, data, dataSize);

    m_potentiometerTurns = (short) turns;
    m_potentiometerTurnsVerified = false;
  }

  /**
   * Configure Soft Position Limits when in Position Controller mode.<br>
   *
   * When controlling position, you can add additional limits on top of the
   * limit switch inputs that are based on the position feedback. If the
   * position limit is reached or the switch is opened, that direction will be
   * disabled.
   *
   * @param forwardLimitPosition The position that, if exceeded, will disable
   *        the forward direction.
   * @param reverseLimitPosition The position that, if exceeded, will disable
   *        the reverse direction.
   */
  public void configSoftPositionLimits(double forwardLimitPosition, double reverseLimitPosition) {
    configLimitMode(LimitMode.SoftPositionLimits);
    configForwardLimit(forwardLimitPosition);
    configReverseLimit(reverseLimitPosition);
  }

  /**
   * Disable Soft Position Limits if previously enabled.<br>
   *
   * Soft Position Limits are disabled by default.
   */
  public void disableSoftPositionLimits() {
    configLimitMode(LimitMode.SwitchInputsOnly);
  }

  /**
   * Set the limit mode for position control mode.<br>
   *
   * Use {@link #configSoftPositionLimits(double, double)} or
   * {@link #disableSoftPositionLimits()} to set this automatically.
   *$
   * @param mode The {@link LimitMode} to use to limit the rotation of the
   *        device.
   * @see LimitMode#SwitchInputsOnly
   * @see LimitMode#SoftPositionLimits
   */
  public void configLimitMode(LimitMode mode) {
    sendMessage(CANJNI.LM_API_CFG_LIMIT_MODE, new byte[] {mode.value}, 1);
  }

  /**
   * Set the position that, if exceeded, will disable the forward direction.
   *
   * Use {@link #configSoftPositionLimits(double, double)} to set this and the
   * {@link LimitMode} automatically.
   *$
   * @param forwardLimitPosition The position that, if exceeded, will disable
   *        the forward direction.
   */
  public void configForwardLimit(double forwardLimitPosition) {
    byte[] data = new byte[8];

    int dataSize = packFXP16_16(data, forwardLimitPosition);
    data[dataSize++] = 1;
    sendMessage(CANJNI.LM_API_CFG_LIMIT_FWD, data, dataSize);

    m_forwardLimit = forwardLimitPosition;
    m_forwardLimitVerified = false;
  }

  /**
   * Set the position that, if exceeded, will disable the reverse direction.
   *
   * Use {@link #configSoftPositionLimits(double, double)} to set this and the
   * {@link LimitMode} automatically.
   *$
   * @param reverseLimitPosition The position that, if exceeded, will disable
   *        the reverse direction.
   */
  public void configReverseLimit(double reverseLimitPosition) {
    byte[] data = new byte[8];

    int dataSize = packFXP16_16(data, reverseLimitPosition);
    data[dataSize++] = 1;
    sendMessage(CANJNI.LM_API_CFG_LIMIT_REV, data, dataSize);

    m_reverseLimit = reverseLimitPosition;
    m_reverseLimitVerified = false;
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
    byte[] data = new byte[8];

    int dataSize = packFXP8_8(data, voltage);
    sendMessage(CANJNI.LM_API_CFG_MAX_VOUT, data, dataSize);

    m_maxOutputVoltage = voltage;
    m_maxOutputVoltageVerified = false;
  }

  /**
   * Configure how long the Jaguar waits in the case of a fault before resuming
   * operation.
   *
   * Faults include over temerature, over current, and bus under voltage. The
   * default is 3.0 seconds, but can be reduced to as low as 0.5 seconds.
   *
   * @param faultTime The time to wait before resuming operation, in seconds.
   */
  public void configFaultTime(float faultTime) {
    byte[] data = new byte[8];

    if (faultTime < 0.5f)
      faultTime = 0.5f;
    else if (faultTime > 3.0f)
      faultTime = 3.0f;

    int dataSize = packINT16(data, (short) (faultTime * 1000.0));
    sendMessage(CANJNI.LM_API_CFG_FAULT_TIME, data, dataSize);

    m_faultTime = faultTime;
    m_faultTimeVerified = false;
  }

  byte m_deviceNumber;
  double m_value = 0.0f;

  // Parameters/configuration
  JaguarControlMode m_controlMode;
  int m_speedReference = CANJNI.LM_REF_NONE;
  int m_positionReference = CANJNI.LM_REF_NONE;
  double m_p = 0.0;
  double m_i = 0.0;
  double m_d = 0.0;
  NeutralMode m_neutralMode = NeutralMode.Jumper;
  short m_encoderCodesPerRev = 0;
  short m_potentiometerTurns = 0;
  LimitMode m_limitMode = LimitMode.SwitchInputsOnly;
  double m_forwardLimit = 0.0;
  double m_reverseLimit = 0.0;
  double m_maxOutputVoltage = kApproxBusVoltage;
  double m_voltageRampRate = 0.0;
  float m_faultTime = 0.0f;

  // Which parameters have been verified since they were last set?
  boolean m_controlModeVerified = true;
  boolean m_speedRefVerified = true;
  boolean m_posRefVerified = true;
  boolean m_pVerified = true;
  boolean m_iVerified = true;
  boolean m_dVerified = true;
  boolean m_neutralModeVerified = true;
  boolean m_encoderCodesPerRevVerified = true;
  boolean m_potentiometerTurnsVerified = true;
  boolean m_forwardLimitVerified = true;
  boolean m_reverseLimitVerified = true;
  boolean m_limitModeVerified = true;
  boolean m_maxOutputVoltageVerified = true;
  boolean m_voltageRampRateVerified = true;
  boolean m_faultTimeVerified = true;

  // Status data
  double m_busVoltage = 0.0f;
  double m_outputVoltage = 0.0f;
  double m_outputCurrent = 0.0f;
  double m_temperature = 0.0f;
  double m_position = 0.0;
  double m_speed = 0.0;
  byte m_limits = (byte) 0;
  short m_faults = (short) 0;
  int m_firmwareVersion = 0;
  byte m_hardwareVersion = (byte) 0;

  // Which periodic status messages have we received at least once?
  boolean m_receivedStatusMessage0 = false;
  boolean m_receivedStatusMessage1 = false;
  boolean m_receivedStatusMessage2 = false;

  static final int kReceiveStatusAttempts = 50;

  boolean m_controlEnabled = true;
  boolean m_stopped = false;

  static void sendMessageHelper(int messageID, byte[] data, int dataSize, int period)
      throws CANMessageNotFoundException {
    final int[] kTrustedMessages =
        {CANJNI.LM_API_VOLT_T_EN, CANJNI.LM_API_VOLT_T_SET, CANJNI.LM_API_SPD_T_EN,
            CANJNI.LM_API_SPD_T_SET, CANJNI.LM_API_VCOMP_T_EN, CANJNI.LM_API_VCOMP_T_SET,
            CANJNI.LM_API_POS_T_EN, CANJNI.LM_API_POS_T_SET, CANJNI.LM_API_ICTRL_T_EN,
            CANJNI.LM_API_ICTRL_T_SET};

    for (byte i = 0; i < kTrustedMessages.length; i++) {
      if ((kFullMessageIDMask & messageID) == kTrustedMessages[i]) {
        // Make sure the data will still fit after adjusting for the token.
        if (dataSize > kMaxMessageDataSize - 2) {
          throw new RuntimeException("CAN message has too much data.");
        }

        ByteBuffer trustedBuffer = ByteBuffer.allocateDirect(dataSize + 2);
        trustedBuffer.put(0, (byte) 0);
        trustedBuffer.put(1, (byte) 0);

        for (byte j = 0; j < dataSize; j++) {
          trustedBuffer.put(j + 2, data[j]);
        }

        CANJNI.FRCNetworkCommunicationCANSessionMuxSendMessage(messageID, trustedBuffer, period);

        return;
      }
    }

    // Use a null pointer for the data buffer if the given array is null
    ByteBuffer buffer;
    if (data != null) {
      buffer = ByteBuffer.allocateDirect(dataSize);
      for (byte i = 0; i < dataSize; i++) {
        buffer.put(i, data[i]);
      }
    } else {
      buffer = null;
    }

    CANJNI.FRCNetworkCommunicationCANSessionMuxSendMessage(messageID, buffer, period);
  }

  /**
   * Send a message to the Jaguar.
   *
   * @param messageID The messageID to be used on the CAN bus (device number is
   *        added internally)
   * @param data The up to 8 bytes of data to be sent with the message
   * @param dataSize Specify how much of the data in "data" to send
   * @param period If positive, tell Network Communications to send the message
   *        every "period" milliseconds.
   */
  protected void sendMessage(int messageID, byte[] data, int dataSize, int period) {
    sendMessageHelper(messageID | m_deviceNumber, data, dataSize, period);
  }

  /**
   * Send a message to the Jaguar, non-periodically
   *
   * @param messageID The messageID to be used on the CAN bus (device number is
   *        added internally)
   * @param data The up to 8 bytes of data to be sent with the message
   * @param dataSize Specify how much of the data in "data" to send
   */
  protected void sendMessage(int messageID, byte[] data, int dataSize) {
    sendMessage(messageID, data, dataSize, CANJNI.CAN_SEND_PERIOD_NO_REPEAT);
  }

  /**
   * Request a message from the Jaguar, but don't wait for it to arrive.
   *
   * @param messageID The message to request
   * @param period If positive, tell Network Communications to request the
   *        message every "period" milliseconds.
   */
  protected void requestMessage(int messageID, int period) {
    sendMessageHelper(messageID | m_deviceNumber, null, 0, period);
  }

  /**
   * Request a message from the Jaguar, but don't wait for it to arrive.
   *
   * @param messageID The message to request
   */
  protected void requestMessage(int messageID) {
    requestMessage(messageID, CANJNI.CAN_SEND_PERIOD_NO_REPEAT);
  }

  /**
   * Get a previously requested message.
   *
   * Jaguar always generates a message with the same message ID when replying.
   *
   * @param messageID The messageID to read from the CAN bus (device number is
   *        added internally)
   * @param data The up to 8 bytes of data that was received with the message
   *
   * @throws CANMessageNotFoundException if there's not new message available
   */
  protected void getMessage(int messageID, int messageMask, byte[] data)
      throws CANMessageNotFoundException {
    messageID |= m_deviceNumber;
    messageID &= CANJNI.CAN_MSGID_FULL_M;

    ByteBuffer targetedMessageID = ByteBuffer.allocateDirect(4);
    targetedMessageID.order(ByteOrder.LITTLE_ENDIAN);
    targetedMessageID.asIntBuffer().put(0, messageID);

    ByteBuffer timeStamp = ByteBuffer.allocateDirect(4);

    // Get the data.
    ByteBuffer dataBuffer =
        CANJNI.FRCNetworkCommunicationCANSessionMuxReceiveMessage(targetedMessageID.asIntBuffer(),
            messageMask, timeStamp);

    if (data != null) {
      for (int i = 0; i < dataBuffer.capacity(); i++) {
        data[i] = dataBuffer.get(i);
      }
    }
  }

  /**
   * Enables periodic status updates from the Jaguar
   */
  protected void setupPeriodicStatus() {
    byte[] data = new byte[8];
    int dataSize;

    // Message 0 returns bus voltage, output voltage, output current, and
    // temperature.
    final byte[] kMessage0Data =
        new byte[] {CANJNI.LM_PSTAT_VOLTBUS_B0, CANJNI.LM_PSTAT_VOLTBUS_B1,
            CANJNI.LM_PSTAT_VOLTOUT_B0, CANJNI.LM_PSTAT_VOLTOUT_B1, CANJNI.LM_PSTAT_CURRENT_B0,
            CANJNI.LM_PSTAT_CURRENT_B1, CANJNI.LM_PSTAT_TEMP_B0, CANJNI.LM_PSTAT_TEMP_B1};

    // Message 1 returns position and speed
    final byte[] kMessage1Data =
        new byte[] {CANJNI.LM_PSTAT_POS_B0, CANJNI.LM_PSTAT_POS_B1, CANJNI.LM_PSTAT_POS_B2,
            CANJNI.LM_PSTAT_POS_B3, CANJNI.LM_PSTAT_SPD_B0, CANJNI.LM_PSTAT_SPD_B1,
            CANJNI.LM_PSTAT_SPD_B2, CANJNI.LM_PSTAT_SPD_B3};

    // Message 2 returns limits and faults
    final byte[] kMessage2Data =
        new byte[] {CANJNI.LM_PSTAT_LIMIT_CLR, CANJNI.LM_PSTAT_FAULT, CANJNI.LM_PSTAT_END,
            (byte) 0, (byte) 0, (byte) 0, (byte) 0, (byte) 0,};

    dataSize = packINT16(data, (short) (kSendMessagePeriod));
    sendMessage(CANJNI.LM_API_PSTAT_PER_EN_S0, data, dataSize);
    sendMessage(CANJNI.LM_API_PSTAT_PER_EN_S1, data, dataSize);
    sendMessage(CANJNI.LM_API_PSTAT_PER_EN_S2, data, dataSize);

    dataSize = 8;
    sendMessage(CANJNI.LM_API_PSTAT_CFG_S0, kMessage0Data, dataSize);
    sendMessage(CANJNI.LM_API_PSTAT_CFG_S1, kMessage1Data, dataSize);
    sendMessage(CANJNI.LM_API_PSTAT_CFG_S2, kMessage2Data, dataSize);
  }

  /**
   * Check for new periodic status updates and unpack them into local variables.
   */
  protected void updatePeriodicStatus() {
    byte[] data = new byte[8];
    int dataSize;

    // Check if a new bus voltage/output voltage/current/temperature message
    // has arrived and unpack the values into the cached member variables
    try {
      getMessage(CANJNI.LM_API_PSTAT_DATA_S0, CANJNI.CAN_MSGID_FULL_M, data);

      m_busVoltage = unpackFXP8_8(new byte[] {data[0], data[1]});
      m_outputVoltage = unpackPercentage(new byte[] {data[2], data[3]}) * m_busVoltage;
      m_outputCurrent = unpackFXP8_8(new byte[] {data[4], data[5]});
      m_temperature = unpackFXP8_8(new byte[] {data[6], data[7]});

      m_receivedStatusMessage0 = true;
    } catch (CANMessageNotFoundException e) {
    }

    // Check if a new position/speed message has arrived and do the same
    try {
      getMessage(CANJNI.LM_API_PSTAT_DATA_S1, CANJNI.CAN_MSGID_FULL_M, data);

      m_position = unpackFXP16_16(new byte[] {data[0], data[1], data[2], data[3]});
      m_speed = unpackFXP16_16(new byte[] {data[4], data[5], data[6], data[7]});

      m_receivedStatusMessage1 = true;
    } catch (CANMessageNotFoundException e) {
    }

    // Check if a new limits/faults message has arrived and do the same
    try {
      getMessage(CANJNI.LM_API_PSTAT_DATA_S2, CANJNI.CAN_MSGID_FULL_M, data);
      m_limits = data[0];
      m_faults = data[1];

      m_receivedStatusMessage2 = true;
    } catch (CANMessageNotFoundException e) {
    }
  }

  /**
   * Update all the motors that have pending sets in the syncGroup.
   *
   * @param syncGroup A bitmask of groups to generate synchronous output.
   */
  public static void updateSyncGroup(byte syncGroup) {
    byte[] data = new byte[8];

    data[0] = syncGroup;

    sendMessageHelper(CANJNI.CAN_MSGID_API_SYNC, data, 1, CANJNI.CAN_SEND_PERIOD_NO_REPEAT);
  }

  /* we are on ARM-LE now, not Freescale so no need to swap */
  private final static void swap16(int x, byte[] buffer) {
    buffer[0] = (byte) (x & 0xff);
    buffer[1] = (byte) ((x >> 8) & 0xff);
  }

  private final static void swap32(int x, byte[] buffer) {
    buffer[0] = (byte) (x & 0xff);
    buffer[1] = (byte) ((x >> 8) & 0xff);
    buffer[2] = (byte) ((x >> 16) & 0xff);
    buffer[3] = (byte) ((x >> 24) & 0xff);
  }

  private static final byte packPercentage(byte[] buffer, double value) {
    if (value < -1.0)
      value = -1.0;
    if (value > 1.0)
      value = 1.0;
    short intValue = (short) (value * 32767.0);
    swap16(intValue, buffer);
    return 2;
  }

  private static final byte packFXP8_8(byte[] buffer, double value) {
    short intValue = (short) (value * 256.0);
    swap16(intValue, buffer);
    return 2;
  }

  private static final byte packFXP16_16(byte[] buffer, double value) {
    int intValue = (int) (value * 65536.0);
    swap32(intValue, buffer);
    return 4;
  }

  private static final byte packINT16(byte[] buffer, short value) {
    swap16(value, buffer);
    return 2;
  }

  private static final byte packINT32(byte[] buffer, int value) {
    swap32(value, buffer);
    return 4;
  }

  /**
   * Unpack 16-bit data from a buffer in little-endian byte order
   *$
   * @param buffer The buffer to unpack from
   * @param offset The offset into he buffer to unpack
   * @return The data that was unpacked
   */
  private static final short unpack16(byte[] buffer, int offset) {
    return (short) ((buffer[offset] & 0xFF) | (short) ((buffer[offset + 1] << 8)) & 0xFF00);
  }

  /**
   * Unpack 32-bit data from a buffer in little-endian byte order
   *$
   * @param buffer The buffer to unpack from
   * @param offset The offset into he buffer to unpack
   * @return The data that was unpacked
   */
  private static final int unpack32(byte[] buffer, int offset) {
    return (buffer[offset] & 0xFF) | ((buffer[offset + 1] << 8) & 0xFF00)
        | ((buffer[offset + 2] << 16) & 0xFF0000) | ((buffer[offset + 3] << 24) & 0xFF000000);
  }

  private static final double unpackPercentage(byte[] buffer) {
    return unpack16(buffer, 0) / 32767.0;
  }

  private static final double unpackFXP8_8(byte[] buffer) {
    return unpack16(buffer, 0) / 256.0;
  }

  private static final double unpackFXP16_16(byte[] buffer) {
    return unpack32(buffer, 0) / 65536.0;
  }

  private static final short unpackINT16(byte[] buffer) {
    return unpack16(buffer, 0);
  }

  private static final int unpackINT32(byte[] buffer) {
    return unpack32(buffer, 0);
  }

  /* Compare floats for equality as fixed point numbers */
  public boolean FXP8_EQ(double a, double b) {
    return (int) (a * 256.0) == (int) (b * 256.0);
  }

  /* Compare floats for equality as fixed point numbers */
  public boolean FXP16_EQ(double a, double b) {
    return (int) (a * 65536.0) == (int) (b * 65536.0);
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
    return "CANJaguar ID " + m_deviceNumber;
  }

  public int getDeviceID() {
    return (int) m_deviceNumber;
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
