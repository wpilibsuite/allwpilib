// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.units.Units.Degrees;
import static edu.wpi.first.units.Units.Microseconds;

import edu.wpi.first.hal.util.AllocationException;
import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.IntegerPublisher;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.PubSubOption;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.Time;

/** This class controls a specific servo hooked up to an ExpansionHub. */
public class ExpansionHubServo implements AutoCloseable {
  private ExpansionHub m_hub;
  private final int m_channel;

  private final IntegerPublisher m_pulseWidthPublisher;
  private final IntegerPublisher m_framePeriodPublisher;
  private final BooleanPublisher m_enabledPublisher;

  private static final double kMaxServoAngle = 180.0;
  private static final double kMinServoAngle = 0.0;

  private static final int kDefaultMaxServoPWM = 2400;
  private static final int kDefaultMinServoPWM = 600;

  private static final int m_minPwm = kDefaultMinServoPWM;
  private static final int m_maxPwm = kDefaultMaxServoPWM;

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
   * <p>Servo values range from 0.0 to 1.0 corresponding to the range of full left to full right.
   *
   * @param value Position from 0.0 to 1.0.
   */
  public void set(double value) {
    value = Math.clamp(value, 0.0, 1.0);

    int rawValue = (int) ((value * getFullRangeScaleFactor()) + m_minPwm);

    m_pulseWidthPublisher.set(rawValue);
  }

  /**
   * Sets the servo angle
   *
   * <p>Servo angles range from 0 to 180 degrees. Use set() with your own scaler for other angle
   * ranges.
   *
   * @param angle Position in angle units. Will be scaled between 0 and 180 degrees
   */
  public void setAngle(Angle angle) {
    double dAngle = angle.in(Degrees);
    if (dAngle < kMinServoAngle) {
      dAngle = kMinServoAngle;
    } else if (dAngle > kMaxServoAngle) {
      dAngle = kMaxServoAngle;
    }

    set((dAngle - kMinServoAngle) / getServoAngleRange());
  }

  private double getFullRangeScaleFactor() {
    return m_maxPwm - m_minPwm;
  }

  private double getServoAngleRange() {
    return kMaxServoAngle - kMinServoAngle;
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
