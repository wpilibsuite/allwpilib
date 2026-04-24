// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.expansionhub;

import static org.wpilib.units.Units.Microseconds;

import org.wpilib.hardware.hal.util.AllocationException;
import org.wpilib.networktables.BooleanPublisher;
import org.wpilib.networktables.IntegerPublisher;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.system.SystemServer;
import org.wpilib.units.measure.Time;

/**
 * This class controls a specific servo in continuous rotation mode hooked up to an ExpansionHub.
 */
public class ExpansionHubCRServo implements AutoCloseable {
  private ExpansionHub m_hub;
  private final int m_channel;

  private boolean m_reversed;

  private final IntegerPublisher m_pulseWidthPublisher;
  private final IntegerPublisher m_framePeriodPublisher;
  private final BooleanPublisher m_enabledPublisher;

  private int m_minPwm = 600;
  private int m_maxPwm = 2400;

  /**
   * Constructs a continuous rotation servo at the requested channel on a specific USB port.
   *
   * @param usbId The USB port ID the hub is connected to
   * @param channel The servo channel
   */
  public ExpansionHubCRServo(int usbId, int channel) {
    m_hub = new ExpansionHub(usbId);
    m_channel = channel;

    if (!m_hub.checkServoChannel(channel)) {
      m_hub.close();
      throw new IllegalArgumentException("Channel " + channel + " out of range");
    }

    if (!m_hub.checkAndReserveServo(channel)) {
      m_hub.close();
      throw new AllocationException("ExpansionHub CR Servo already allocated");
    }

    m_hub.reportUsage("ExHubCRServo[" + channel + "]", "ExHubCRServo");

    NetworkTableInstance systemServer = SystemServer.getSystemServer();

    PubSubOption[] options =
        new PubSubOption[] {
          PubSubOption.SEND_ALL, PubSubOption.KEEP_DUPLICATES, PubSubOption.periodic(0.005)
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

  /**
   * Set the servo throttle.
   *
   * <p>Throttle values range from -1.0 to 1.0 corresponding to full reverse to full forward.
   *
   * @param value Throttle from -1.0 to 1.0.
   */
  public void setThrottle(double value) {
    value = Math.clamp(value, -1.0, 1.0);
    value = (value + 1.0) / 2.0;

    if (m_reversed) {
      value = 1.0 - value;
    }

    int rawValue = (int) ((value * getFullRangeScaleFactor()) + m_minPwm);

    setPulseWidth(Microseconds.of(rawValue));
  }

  /**
   * Sets the raw pulse width output on the servo.
   *
   * @param pulseWidth Pulse width
   */
  public void setPulseWidth(Time pulseWidth) {
    setEnabled(true);
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
   * <p>This will reverse setThrottle().
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

  private double getFullRangeScaleFactor() {
    return m_maxPwm - m_minPwm;
  }
}
