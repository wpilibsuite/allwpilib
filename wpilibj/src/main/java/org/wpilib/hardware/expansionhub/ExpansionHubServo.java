// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.expansionhub;

import static org.wpilib.units.Units.Degrees;
import static org.wpilib.units.Units.Microseconds;

import org.wpilib.hardware.hal.util.AllocationException;
import org.wpilib.networktables.BooleanPublisher;
import org.wpilib.networktables.IntegerPublisher;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.system.SystemServer;
import org.wpilib.units.measure.Angle;
import org.wpilib.units.measure.Time;

/** This class controls a specific servo hooked up to an ExpansionHub. */
public class ExpansionHubServo implements AutoCloseable {
  private ExpansionHub m_hub;
  private final int m_channel;

  private boolean m_reversed;
  private boolean m_continousMode;

  private final IntegerPublisher m_pulseWidthPublisher;
  private final IntegerPublisher m_framePeriodPublisher;
  private final BooleanPublisher m_enabledPublisher;

  private double m_maxServoAngle = 180.0;
  private double m_minServoAngle;

  private int m_minPwm = 600;
  private int m_maxPwm = 2400;

  /**
   * Constructs a servo at the requested channel on a specific USB port.
   *
   * @param usbId The USB port ID the hub is connected to
   * @param channel The servo channel
   */
  public ExpansionHubServo(int usbId, int channel) {
    m_hub = new ExpansionHub(usbId);
    m_channel = channel;

    if (!m_hub.checkServoChannel(channel)) {
      m_hub.close();
      throw new IllegalArgumentException("Channel " + channel + " out of range");
    }

    if (!m_hub.checkAndReserveServo(channel)) {
      m_hub.close();
      throw new AllocationException("ExpansionHub Servo already allocated");
    }

    m_hub.reportUsage("ExHubServo[" + channel + "]", "ExHubServo");

    NetworkTableInstance systemServer = SystemServer.getSystemServer();

    PubSubOption[] options =
        new PubSubOption[] {
          PubSubOption.sendAll(true),
          PubSubOption.keepDuplicates(true),
          PubSubOption.periodic(0.005)
        };

    m_pulseWidthPublisher =
        systemServer
            .getIntegerTopic("/rhsp/" + usbId + "/servo" + channel + "/pulseWidth")
            .publish(options);

    m_pulseWidthPublisher.set(1500);

    m_framePeriodPublisher =
        systemServer
            .getIntegerTopic("/rhsp/" + usbId + "/servo" + channel + "/framePeriod")
            .publish(options);

    m_framePeriodPublisher.set(20000);

    m_enabledPublisher =
        systemServer
            .getBooleanTopic("/rhsp/" + usbId + "/servo" + channel + "/enabled")
            .publish(options);
  }

  /**
   * Set the servo position.
   *
   * <p>Servo values range from 0.0 to 1.0 corresponding to the range of full left to full right. If
   * continuous rotation mode is enabled, the range is -1.0 to 1.0.
   *
   * @param value Position from 0.0 to 1.0 (-1 to 1 in CR mode).
   */
  public void set(double value) {
    if (m_continousMode) {
      value = Math.clamp(value, -1.0, 1.0);
      value = (value + 1.0) / 2.0;
    }

    value = Math.clamp(value, 0.0, 1.0);

    if (m_reversed) {
      value = 1.0 - value;
    }

    int rawValue = (int) ((value * getFullRangeScaleFactor()) + m_minPwm);

    m_pulseWidthPublisher.set(rawValue);
  }

  /**
   * Sets the servo angle
   *
   * <p>Servo angles range defaults to 0 to 180 degrees, but can be changed with setAngleRange().
   *
   * @param angle Position in angle units. Will be scaled between the current angle range.
   */
  public void setAngle(Angle angle) {
    double dAngle = angle.in(Degrees);
    if (dAngle < m_minServoAngle) {
      dAngle = m_minServoAngle;
    } else if (dAngle > m_maxServoAngle) {
      dAngle = m_maxServoAngle;
    }

    set((dAngle - m_minServoAngle) / getServoAngleRange());
  }

  private double getFullRangeScaleFactor() {
    return m_maxPwm - m_minPwm;
  }

  private double getServoAngleRange() {
    return m_maxServoAngle - m_minServoAngle;
  }

  /**
   * Sets the raw pulse width output on the servo.
   *
   * @param pulseWidth Pulse width
   */
  public void setPulseWidth(Time pulseWidth) {
    m_pulseWidthPublisher.set((long) pulseWidth.in(Microseconds));
  }

  /**
   * Sets if the servo output is enabled or not. Defaults to false.
   *
   * @param enabled True to enable, false to disable
   */
  public void setEnabled(boolean enabled) {
    m_enabledPublisher.set(enabled);
  }

  /**
   * Sets the frame period for the servo. Defaults to 20ms.
   *
   * @param framePeriod The frame period
   */
  public void setFramePeriod(Time framePeriod) {
    m_framePeriodPublisher.set((long) framePeriod.in(Microseconds));
  }

  /**
   * Gets if the underlying ExpansionHub is connected.
   *
   * @return True if hub is connected, otherwise false
   */
  public boolean isHubConnected() {
    return m_hub.isHubConnected();
  }

  /**
   * Sets whether the servo is reversed.
   *
   * <p>This will reverse both set() and setAngle().
   *
   * @param reversed True to reverse, false for normal
   */
  public void setReversed(boolean reversed) {
    m_reversed = reversed;
  }

  /**
   * Sets the PWM range for the servo. By default, this is 600 to 2400 microseconds.
   *
   * <p>Maximum must be greater than minimum.
   *
   * @param minPwm Minimum PWM
   * @param maxPwm Maximum PWM
   */
  public void setPWMRange(int minPwm, int maxPwm) {
    if (maxPwm <= minPwm) {
      throw new IllegalArgumentException("Maximum PWM must be greater than minimum PWM");
    }
    m_minPwm = minPwm;
    m_maxPwm = maxPwm;
  }

  /**
   * Sets the angle range for the setAngle call. By default, this is 0 to 180 degrees.
   *
   * <p>Maximum angle must be greater than minimum angle.
   *
   * @param minAngle Minimum angle
   * @param maxAngle Maximum angle
   */
  public void setAngleRange(double minAngle, double maxAngle) {
    if (maxAngle <= minAngle) {
      throw new IllegalArgumentException("Maximum angle must be greater than minimum angle");
    }
    m_minServoAngle = minAngle;
    m_maxServoAngle = maxAngle;
  }

  /**
   * Enables or disables continuous rotation mode.
   *
   * <p>In continuous rotation mode, the servo will interpret Set() commands to between -1.0 and
   * 1.0, instead of 0.0 to 1.0.
   *
   * @param enable True to enable continuous rotation mode, false to disable
   */
  public void setContinousRotationMode(boolean enable) {
    m_continousMode = enable;
  }

  /** Closes a servo so another instance can be constructed. */
  @Override
  public void close() {
    m_hub.unreserveServo(m_channel);
    m_hub.close();
    m_hub = null;

    m_pulseWidthPublisher.close();
    m_framePeriodPublisher.close();
    m_enabledPublisher.close();
  }
}
