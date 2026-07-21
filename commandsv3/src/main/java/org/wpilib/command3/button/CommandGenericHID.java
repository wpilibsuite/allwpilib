// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import java.util.HashMap;
import java.util.IdentityHashMap;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.Trigger;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.driverstation.GenericHID;
import org.wpilib.driverstation.POVDirection;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.event.EventLoop;
import org.wpilib.math.util.Pair;

/**
 * A version of {@link GenericHID} with {@link Trigger} factories for command-based.
 *
 * @see GenericHID
 */
public final class CommandGenericHID {
  private static final Lock m_hidsLock = new ReentrantLock();
  private static final Map<Scheduler, CommandGenericHID[]> m_hids = new IdentityHashMap<>();

  private final Scheduler m_scheduler;
  private final GenericHID m_hid;
  private final Map<EventLoop, Map<Integer, Trigger>> m_buttonCache = new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, Trigger>> m_axisLessThanCache =
      new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, Trigger>> m_axisGreaterThanCache =
      new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, Trigger>>
      m_axisMagnitudeGreaterThanCache = new HashMap<>();
  private final Map<EventLoop, Map<Integer, Trigger>> m_povCache = new HashMap<>();

  /**
   * Construct an instance of a device.
   *
   * @param scheduler The scheduler that should execute the triggered commands.
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public CommandGenericHID(Scheduler scheduler, int port) {
    m_scheduler = scheduler;
    m_hid = DriverStation.getGenericHID(port);
  }

  /**
   * Construct an instance of a device with a GenericHID object.
   *
   * @param scheduler The scheduler that should execute the triggered commands.
   * @param hid The GenericHID object to use for this command HID.
   */
  public CommandGenericHID(Scheduler scheduler, GenericHID hid) {
    m_scheduler = scheduler;
    m_hid = hid;
  }

  /**
   * Construct an instance of a device with a GenericHID object.
   *
   * @param hid The GenericHID object to use for this command HID.
   */
  public CommandGenericHID(GenericHID hid) {
    this(Scheduler.getDefault(), hid);
  }

  /**
   * Gets the CommandGenericHID object for the given scheduler and port. CommandGenericHID objects
   * are cached, so this will always return the same object for the same scheduler and port.
   *
   * @param scheduler The scheduler that should execute the triggered commands.
   * @param port The port index on the Driver Station that the device is plugged into.
   * @return The CommandGenericHID object for the given scheduler and port.
   */
  public static CommandGenericHID getCommandGenericHID(Scheduler scheduler, int port) {
    DriverStation.getGenericHID(port);
    m_hidsLock.lock();
    try {
      CommandGenericHID[] hids =
          m_hids.computeIfAbsent(
              scheduler, k -> new CommandGenericHID[DriverStationBackend.JOYSTICK_PORTS]);
      CommandGenericHID toRet = hids[port];
      if (toRet == null) {
        toRet = new CommandGenericHID(scheduler, port);
        hids[port] = toRet;
      }
      return toRet;
    } finally {
      m_hidsLock.unlock();
    }
  }

  /**
   * Gets the CommandGenericHID object for the given port using the default scheduler.
   * CommandGenericHID objects are cached, so this will always return the same object for the same
   * port.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   * @return The CommandGenericHID object for the given port.
   */
  public static CommandGenericHID getCommandGenericHID(int port) {
    return getCommandGenericHID(Scheduler.getDefault(), port);
  }

  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public CommandGenericHID(int port) {
    this(Scheduler.getDefault(), port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  public GenericHID getHID() {
    return m_hid;
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @return an event instance representing the button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #button(int, EventLoop)
   */
  public Trigger button(int button) {
    return button(button, m_scheduler.getDefaultEventLoop());
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached to the given loop.
   */
  public Trigger button(int button, EventLoop loop) {
    var cache = m_buttonCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        button, k -> new Trigger(m_scheduler, loop, () -> m_hid.getRawButton(k)));
  }

  /**
   * Constructs a Trigger instance based around this angle of the default (index 0) POV on the HID,
   * attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler button loop}.
   *
   * @param angle POV angle
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(POVDirection angle) {
    return pov(0, angle, m_scheduler.getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance based around this angle of a POV on the HID.
   *
   * @param pov index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle
   * @param loop the event loop instance to attach the event to. Defaults to {@link
   *     Scheduler#getDefaultEventLoop() the default command scheduler button loop}.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(int pov, POVDirection angle, EventLoop loop) {
    var cache = m_povCache.computeIfAbsent(loop, k -> new HashMap<>());
    // angle.value is a 4 bit bitfield
    return cache.computeIfAbsent(
        pov * 16 + angle.value,
        k -> new Trigger(m_scheduler, loop, () -> m_hid.getPOV(pov) == angle));
  }

  /**
   * Constructs a Trigger instance based around the 0 degree angle (up) of the default (index 0) POV
   * on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler
   * button loop}.
   *
   * @return a Trigger instance based around the 0 degree angle of a POV on the HID.
   */
  public Trigger povUp() {
    return pov(POVDirection.UP);
  }

  /**
   * Constructs a Trigger instance based around the 45 degree angle (right up) of the default (index
   * 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 45 degree angle of a POV on the HID.
   */
  public Trigger povUpRight() {
    return pov(POVDirection.UP_RIGHT);
  }

  /**
   * Constructs a Trigger instance based around the 90 degree angle (right) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 90 degree angle of a POV on the HID.
   */
  public Trigger povRight() {
    return pov(POVDirection.RIGHT);
  }

  /**
   * Constructs a Trigger instance based around the 135 degree angle (right down) of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the 135 degree angle of a POV on the HID.
   */
  public Trigger povDownRight() {
    return pov(POVDirection.DOWN_RIGHT);
  }

  /**
   * Constructs a Trigger instance based around the 180 degree angle (down) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 180 degree angle of a POV on the HID.
   */
  public Trigger povDown() {
    return pov(POVDirection.DOWN);
  }

  /**
   * Constructs a Trigger instance based around the 225 degree angle (down left) of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the 225 degree angle of a POV on the HID.
   */
  public Trigger povDownLeft() {
    return pov(POVDirection.DOWN_LEFT);
  }

  /**
   * Constructs a Trigger instance based around the 270 degree angle (left) of the default (index 0)
   * POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 270 degree angle of a POV on the HID.
   */
  public Trigger povLeft() {
    return pov(POVDirection.LEFT);
  }

  /**
   * Constructs a Trigger instance based around the 315 degree angle (left up) of the default (index
   * 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 315 degree angle of a POV on the HID.
   */
  public Trigger povUpLeft() {
    return pov(POVDirection.UP_LEFT);
  }

  /**
   * Constructs a Trigger instance based around the center (not pressed) position of the default
   * (index 0) POV on the HID, attached to {@link Scheduler#getDefaultEventLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the center position of a POV on the HID.
   */
  public Trigger povCenter() {
    return pov(POVDirection.CENTER);
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler button loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this trigger should return true.
   * @return a Trigger instance that is true when the axis value is less than the provided
   *     threshold.
   */
  public Trigger axisLessThan(int axis, double threshold) {
    return axisLessThan(axis, threshold, m_scheduler.getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this trigger should return true.
   * @param loop the event loop instance to attach the trigger to
   * @return a Trigger instance that is true when the axis value is less than the provided
   *     threshold.
   */
  public Trigger axisLessThan(int axis, double threshold, EventLoop loop) {
    var cache = m_axisLessThanCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        Pair.of(axis, threshold),
        k -> new Trigger(m_scheduler, loop, () -> getRawAxis(axis) < threshold));
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is less than {@code threshold},
   * attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler button loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @return a Trigger instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public Trigger axisGreaterThan(int axis, double threshold) {
    return axisGreaterThan(axis, threshold, m_scheduler.getDefaultEventLoop());
  }

  /**
   * Constructs a Trigger instance that is true when the axis value is greater than {@code
   * threshold}, attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @param loop the event loop instance to attach the trigger to.
   * @return a Trigger instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public Trigger axisGreaterThan(int axis, double threshold, EventLoop loop) {
    var cache = m_axisGreaterThanCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        Pair.of(axis, threshold),
        k -> new Trigger(m_scheduler, loop, () -> getRawAxis(axis) > threshold));
  }

  /**
   * Constructs a Trigger instance that is true when the axis magnitude value is greater than {@code
   * threshold}, attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @param loop the event loop instance to attach the trigger to.
   * @return a Trigger instance that is true when the axis magnitude value is greater than the
   *     provided threshold.
   */
  public Trigger axisMagnitudeGreaterThan(int axis, double threshold, EventLoop loop) {
    var cache = m_axisMagnitudeGreaterThanCache.computeIfAbsent(loop, k -> new HashMap<>());
    return cache.computeIfAbsent(
        Pair.of(axis, threshold),
        k -> new Trigger(m_scheduler, loop, () -> Math.abs(getRawAxis(axis)) > threshold));
  }

  /**
   * Constructs a Trigger instance that is true when the axis magnitude value is greater than {@code
   * threshold}, attached to {@link Scheduler#getDefaultEventLoop() the default command scheduler
   * button loop}.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this trigger should return true.
   * @return a Trigger instance that is true when the deadbanded axis value is active (non-zero).
   */
  public Trigger axisMagnitudeGreaterThan(int axis, double threshold) {
    return axisMagnitudeGreaterThan(axis, threshold, m_scheduler.getDefaultEventLoop());
  }

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return The value of the axis.
   */
  public double getRawAxis(int axis) {
    return m_hid.getRawAxis(axis);
  }

  /**
   * Set the rumble output for the HID. The DS currently supports 2 rumble values, left rumble and
   * right rumble.
   *
   * @param type Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
  public void setRumble(GenericHID.RumbleType type, double value) {
    m_hid.setRumble(type, value);
  }

  private Command rumble(
      Mechanism mechanism, String name, GenericHID.RumbleType type, double value) {
    return mechanism
        .run(
            coroutine -> {
              setRumble(type, value);
              coroutine.park();
            })
        .whenCanceled(() -> setRumble(type, 0))
        .named(name);
  }

  // Rumble mutexes
  private final Mechanism m_leftRumble = new Mechanism() {
    @Override
    public String getName() {
      return "Controller " + m_hid.getPort() + " Left Rumble";
    }
  };
  private final Mechanism m_rightRumble = new Mechanism() {
    @Override
    public String getName() {
      return "Controller " + m_hid.getPort() + " Right Rumble";
    }
  };
  private final Mechanism m_leftTriggerRumble = new Mechanism() {
    @Override
    public String getName() {
      return "Controller " + m_hid.getPort() + " Left Trigger Rumble";
    }
  };
  private final Mechanism m_rightTriggerRumble = new Mechanism() {
    @Override
    public String getName() {
      return "Controller " + m_hid.getPort() + " Right Trigger Rumble";
    }
  };

  /**
   * Run the left rumble motor. On most controllers, this is the low-frequency motor.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the left rumble motor at the given value until interrupted.
   */
  public Command rumbleLeft(double value) {
    return rumble(m_leftRumble, "Left Rumble", GenericHID.RumbleType.LEFT_RUMBLE, value);
  }

  /**
   * Run the right rumble motor. On most controllers, this is the high-frequency motor.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the right rumble motor at the given value until interrupted.
   */
  public Command rumbleRight(double value) {
    return rumble(m_rightRumble, "Right Rumble", GenericHID.RumbleType.RIGHT_RUMBLE, value);
  }

  /**
   * Run both rumble motors.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the rumble motors at the given value until interrupted.
   */
  public Command rumbleBoth(double value) {
    return Command.parallel(rumbleLeft(value), rumbleRight(value)).named("Both Rumble");
  }

  /**
   * Run the left trigger rumble motor, on controllers that have one.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the left trigger rumble motor at the given value until
   *     interrupted.
   */
  public Command rumbleLeftTrigger(double value) {
    return rumble(
        m_leftTriggerRumble,
        "Left Trigger Rumble",
        GenericHID.RumbleType.LEFT_TRIGGER_RUMBLE,
        value);
  }

  /**
   * Run the right trigger rumble motor, on controllers that have one.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run the right trigger rumble motor at the given value until
   *     interrupted.
   */
  public Command rumbleRightTrigger(double value) {
    return rumble(
        m_rightTriggerRumble,
        "Right Trigger Rumble",
        GenericHID.RumbleType.RIGHT_TRIGGER_RUMBLE,
        value);
  }

  /**
   * Run both trigger rumble motors, on controllers that have them.
   *
   * @param value The normalized value (0 to 1) to set the rumble to
   * @return A command that will run both trigger rumble motors at the given value until
   *     interrupted.
   */
  public Command rumbleTriggers(double value) {
    return Command.parallel(rumbleLeftTrigger(value), rumbleRightTrigger(value))
        .named("Both Trigger Rumble");
  }

  // LED mutex
  private final Mechanism m_led = new Mechanism() {
    @Override
    public String getName() {
      return "Controller " + m_hid.getPort() + " LED";
    }
  };

  /**
   * Set the LEDs, on controllers that have them.
   *
   * @param r The red value (0-255)
   * @param g The green value (0-255)
   * @param b The blue value (0-255)
   * @return A command that will set the LEDs to the given values until interrupted.
   */
  public Command setLeds(int r, int g, int b) {
    return m_led
        .run(
            coroutine -> {
              m_hid.setLeds(r, g, b);
              coroutine.park();
            })
        .whenCanceled(() -> m_hid.setLeds(0, 0, 0))
        .named("Set LED (" + r + ", " + g + ", " + b + ")");
  }

  /**
   * Get if the HID is connected.
   *
   * @return true if the HID is connected
   */
  public boolean isConnected() {
    return m_hid.isConnected();
  }

  /**
   * Gets the scheduler that should execute the triggered commands. This scheduler is set in the
   * constructor, defaulting to {@link Scheduler#getDefault()} if one was not provided.
   *
   * @return the scheduler that should execute the triggered commands
   */
  Scheduler getScheduler() {
    return m_scheduler;
  }
}
