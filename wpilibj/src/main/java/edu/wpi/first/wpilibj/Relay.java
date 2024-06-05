// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.RelayJNI;
import edu.wpi.first.hal.util.HalHandleException;
import edu.wpi.first.hal.util.UncleanStatusException;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import java.util.Arrays;
import java.util.Optional;

/**
 * Class for VEX Robotics Spike style relay outputs. Relays are intended to be connected to Spikes
 * or similar relays. The relay channels controls a pair of channels that are either both off, one
 * on, the other on, or both on. This translates into two Spike outputs at 0v, one at 12v and one at
 * 0v, one at 0v and the other at 12v, or two Spike outputs at 12V. This allows off, full forward,
 * or full reverse control of motors without variable speed. It also allows the two channels
 * (forward and reverse) to be used independently for something that does not care about voltage
 * polarity (like a solenoid).
 */
public class Relay extends MotorSafety implements Sendable, AutoCloseable {
  /**
   * This class represents errors in trying to set relay values contradictory to the direction to
   * which the relay is set.
   */
  public static class InvalidValueException extends RuntimeException {
    /**
     * Create a new exception with the given message.
     *
     * @param message the message to pass with the exception
     */
    public InvalidValueException(String message) {
      super(message);
    }
  }

  /** The state to drive a Relay to. */
  public enum Value {
    /** Off. */
    kOff("Off"),
    /** On. */
    kOn("On"),
    /** Forward. */
    kForward("Forward"),
    /** Reverse. */
    kReverse("Reverse");

    private final String m_prettyValue;

    Value(String prettyValue) {
      m_prettyValue = prettyValue;
    }

    /**
     * Returns the pretty string representation of the value.
     *
     * @return The pretty string representation of the value.
     */
    public String getPrettyValue() {
      return m_prettyValue;
    }

    /**
     * Returns the value for a given pretty string.
     *
     * @param value The pretty string.
     * @return The value or an empty optional if there is no corresponding value.
     */
    public static Optional<Value> getValueOf(String value) {
      return Arrays.stream(Value.values()).filter(v -> v.m_prettyValue.equals(value)).findFirst();
    }
  }

  /** The Direction(s) that a relay is configured to operate in. */
  public enum Direction {
    /** Both directions are valid. */
    kBoth,
    /** Only forward is valid. */
    kForward,
    /** Only reverse is valid. */
    kReverse
  }

  private final int m_channel;

  private int m_forwardHandle;
  private int m_reverseHandle;

  private Direction m_direction;

  /**
   * Common relay initialization method. This code is common to all Relay constructors and
   * initializes the relay and reserves all resources that need to be locked. Initially the relay is
   * set to both lines at 0v.
   */
  private void initRelay() {
    SensorUtil.checkRelayChannel(m_channel);

    int portHandle = HAL.getPort((byte) m_channel);
    if (m_direction == Direction.kBoth || m_direction == Direction.kForward) {
      m_forwardHandle = RelayJNI.initializeRelayPort(portHandle, true);
      HAL.report(tResourceType.kResourceType_Relay, m_channel + 1);
    }
    if (m_direction == Direction.kBoth || m_direction == Direction.kReverse) {
      m_reverseHandle = RelayJNI.initializeRelayPort(portHandle, false);
      HAL.report(tResourceType.kResourceType_Relay, m_channel + 128);
    }

    setSafetyEnabled(false);

    SendableRegistry.addLW(this, "Relay", m_channel);
  }

  /**
   * Relay constructor given a channel.
   *
   * @param channel The channel number for this relay (0 - 3).
   * @param direction The direction that the Relay object will control.
   */
  @SuppressWarnings("this-escape")
  public Relay(final int channel, Direction direction) {
    m_channel = channel;
    m_direction = requireNonNullParam(direction, "direction", "Relay");
    initRelay();
    set(Value.kOff);
  }

  /**
   * Relay constructor given a channel, allowing both directions.
   *
   * @param channel The channel number for this relay (0 - 3).
   */
  public Relay(final int channel) {
    this(channel, Direction.kBoth);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    freeRelay();
  }

  private void freeRelay() {
    try {
      RelayJNI.setRelay(m_forwardHandle, false);
    } catch (UncleanStatusException | HalHandleException ignored) {
      // do nothing. Ignore
    }
    try {
      RelayJNI.setRelay(m_reverseHandle, false);
    } catch (UncleanStatusException | HalHandleException ignored) {
      // do nothing. Ignore
    }

    RelayJNI.freeRelayPort(m_forwardHandle);
    RelayJNI.freeRelayPort(m_reverseHandle);

    m_forwardHandle = 0;
    m_reverseHandle = 0;
  }

  /**
   * Set the relay state.
   *
   * <p>Valid values depend on which directions of the relay are controlled by the object.
   *
   * <p>When set to kBothDirections, the relay can be set to any of the four states: 0v-0v, 12v-0v,
   * 0v-12v, 12v-12v
   *
   * <p>When set to kForwardOnly or kReverseOnly, you can specify the constant for the direction, or
   * you can simply specify kOff and kOn. Using only kOff and kOn is recommended.
   *
   * @param value The state to set the relay.
   */
  public void set(Value value) {
    switch (value) {
      case kOff -> {
        if (m_direction == Direction.kBoth || m_direction == Direction.kForward) {
          RelayJNI.setRelay(m_forwardHandle, false);
        }
        if (m_direction == Direction.kBoth || m_direction == Direction.kReverse) {
          RelayJNI.setRelay(m_reverseHandle, false);
        }
      }
      case kOn -> {
        if (m_direction == Direction.kBoth || m_direction == Direction.kForward) {
          RelayJNI.setRelay(m_forwardHandle, true);
        }
        if (m_direction == Direction.kBoth || m_direction == Direction.kReverse) {
          RelayJNI.setRelay(m_reverseHandle, true);
        }
      }
      case kForward -> {
        if (m_direction == Direction.kReverse) {
          throw new InvalidValueException(
              "A relay configured for reverse cannot be set to " + "forward");
        }
        if (m_direction == Direction.kBoth || m_direction == Direction.kForward) {
          RelayJNI.setRelay(m_forwardHandle, true);
        }
        if (m_direction == Direction.kBoth) {
          RelayJNI.setRelay(m_reverseHandle, false);
        }
      }
      case kReverse -> {
        if (m_direction == Direction.kForward) {
          throw new InvalidValueException(
              "A relay configured for forward cannot be set to " + "reverse");
        }
        if (m_direction == Direction.kBoth) {
          RelayJNI.setRelay(m_forwardHandle, false);
        }
        if (m_direction == Direction.kBoth || m_direction == Direction.kReverse) {
          RelayJNI.setRelay(m_reverseHandle, true);
        }
      }
      default -> {
        // Cannot hit this, limited by Value enum
      }
    }
  }

  /**
   * Get the Relay State.
   *
   * <p>Gets the current state of the relay.
   *
   * <p>When set to kForwardOnly or kReverseOnly, value is returned as kOn/kOff not
   * kForward/kReverse (per the recommendation in Set)
   *
   * @return The current state of the relay as a Relay::Value
   */
  public Value get() {
    if (m_direction == Direction.kForward) {
      if (RelayJNI.getRelay(m_forwardHandle)) {
        return Value.kOn;
      } else {
        return Value.kOff;
      }
    } else if (m_direction == Direction.kReverse) {
      if (RelayJNI.getRelay(m_reverseHandle)) {
        return Value.kOn;
      } else {
        return Value.kOff;
      }
    } else {
      if (RelayJNI.getRelay(m_forwardHandle)) {
        if (RelayJNI.getRelay(m_reverseHandle)) {
          return Value.kOn;
        } else {
          return Value.kForward;
        }
      } else {
        if (RelayJNI.getRelay(m_reverseHandle)) {
          return Value.kReverse;
        } else {
          return Value.kOff;
        }
      }
    }
  }

  /**
   * Get the channel number.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_channel;
  }

  @Override
  public void stopMotor() {
    set(Value.kOff);
  }

  @Override
  public String getDescription() {
    return "Relay ID " + getChannel();
  }

  /**
   * Set the Relay Direction.
   *
   * <p>Changes which values the relay can be set to depending on which direction is used
   *
   * <p>Valid inputs are kBothDirections, kForwardOnly, and kReverseOnly
   *
   * @param direction The direction for the relay to operate in
   */
  @SuppressWarnings("this-escape")
  public void setDirection(Direction direction) {
    requireNonNullParam(direction, "direction", "setDirection");
    if (m_direction == direction) {
      return;
    }

    freeRelay();
    m_direction = direction;
    initRelay();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Relay");
    builder.setActuator(true);
    builder.setSafeState(() -> set(Value.kOff));
    builder.addStringProperty(
        "Value",
        () -> get().getPrettyValue(),
        value -> set(Value.getValueOf(value).orElse(Value.kOff)));
  }
}
