// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Pair;
import edu.wpi.first.wpilibj.event.BooleanEvent;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.HashMap;
import java.util.Map;

/**
 * Handle input from standard HID devices connected to the Driver Station.
 *
 * <p>This class handles standard input that comes from the Driver Station. Each time a value is
 * requested the most recent value is returned. There is a single class instance for each device and
 * the mapping of ports to hardware buttons depends on the code in the Driver Station.
 */
public class GenericHID {
  /** Represents a rumble output on the Joystick. */
  public enum RumbleType {
    /** Left rumble motor. */
    kLeftRumble,
    /** Right rumble motor. */
    kRightRumble,
    /** Both left and right rumble motors. */
    kBothRumble
  }

  /** USB HID interface type. */
  public enum HIDType {
    /** Unknown. */
    kUnknown(-1),
    /** XInputUnknown. */
    kXInputUnknown(0),
    /** XInputGamepad. */
    kXInputGamepad(1),
    /** XInputWheel. */
    kXInputWheel(2),
    /** XInputArcadeStick. */
    kXInputArcadeStick(3),
    /** XInputFlightStick. */
    kXInputFlightStick(4),
    /** XInputDancePad. */
    kXInputDancePad(5),
    /** XInputGuitar. */
    kXInputGuitar(6),
    /** XInputGuitar2. */
    kXInputGuitar2(7),
    /** XInputDrumKit. */
    kXInputDrumKit(8),
    /** XInputGuitar3. */
    kXInputGuitar3(11),
    /** XInputArcadePad. */
    kXInputArcadePad(19),
    /** HIDJoystick. */
    kHIDJoystick(20),
    /** HIDGamepad. */
    kHIDGamepad(21),
    /** HIDDriving. */
    kHIDDriving(22),
    /** HIDFlight. */
    kHIDFlight(23),
    /** HID1stPerson. */
    kHID1stPerson(24);

    /** HIDType value. */
    public final int value;

    @SuppressWarnings("PMD.UseConcurrentHashMap")
    private static final Map<Integer, HIDType> map = new HashMap<>();

    HIDType(int value) {
      this.value = value;
    }

    static {
      for (HIDType hidType : HIDType.values()) {
        map.put(hidType.value, hidType);
      }
    }

    /**
     * Creates an HIDType with the given value.
     *
     * @param value HIDType's value.
     * @return HIDType with the given value.
     */
    public static HIDType of(int value) {
      return map.get(value);
    }
  }

  private final int m_port;
  private int m_outputs;
  private int m_leftRumble;
  private int m_rightRumble;
  private final Map<EventLoop, Map<Integer, BooleanEvent>> m_buttonCache = new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, BooleanEvent>> m_axisLessThanCache =
      new HashMap<>();
  private final Map<EventLoop, Map<Pair<Integer, Double>, BooleanEvent>> m_axisGreaterThanCache =
      new HashMap<>();
  private final Map<EventLoop, Map<Integer, BooleanEvent>> m_povCache = new HashMap<>();

  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public GenericHID(int port) {
    m_port = port;
  }

  /**
   * Get the button value (starting at button 1).
   *
   * <p>The buttons are returned in a single 16 bit value with one bit representing the state of
   * each button. The appropriate button is returned as a boolean value.
   *
   * <p>This method returns true if the button is being held down at the time that this method is
   * being called.
   *
   * @param button The button number to be read (starting at 1)
   * @return The state of the button.
   */
  public boolean getRawButton(int button) {
    return DriverStation.getStickButton(m_port, (byte) button);
  }

  /**
   * Whether the button was pressed since the last check. Button indexes begin at 1.
   *
   * <p>This method returns true if the button went from not pressed to held down since the last
   * time this method was called. This is useful if you only want to call a function once when you
   * press the button.
   *
   * @param button The button index, beginning at 1.
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRawButtonPressed(int button) {
    return DriverStation.getStickButtonPressed(m_port, (byte) button);
  }

  /**
   * Whether the button was released since the last check. Button indexes begin at 1.
   *
   * <p>This method returns true if the button went from held down to not pressed since the last
   * time this method was called. This is useful if you only want to call a function once when you
   * release the button.
   *
   * @param button The button index, beginning at 1.
   * @return Whether the button was released since the last check.
   */
  public boolean getRawButtonReleased(int button) {
    return DriverStation.getStickButtonReleased(m_port, button);
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached to the given loop.
   */
  public BooleanEvent button(int button, EventLoop loop) {
    synchronized (m_buttonCache) {
      var cache = m_buttonCache.computeIfAbsent(loop, k -> new HashMap<>());
      return cache.computeIfAbsent(button, k -> new BooleanEvent(loop, () -> getRawButton(k)));
    }
  }

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return The value of the axis.
   */
  public double getRawAxis(int axis) {
    return DriverStation.getStickAxis(m_port, axis);
  }

  /**
   * Get the angle in degrees of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (e.g. right is 90,
   * upper-left is 315).
   *
   * @param pov The index of the POV to read (starting at 0). Defaults to 0.
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  public int getPOV(int pov) {
    return DriverStation.getStickPOV(m_port, pov);
  }

  /**
   * Get the angle in degrees of the default POV (index 0) on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (e.g. right is 90,
   * upper-left is 315).
   *
   * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
   */
  public int getPOV() {
    return getPOV(0);
  }

  /**
   * Constructs a BooleanEvent instance based around this angle of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (eg right is 90,
   * upper-left is 315).
   *
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around this angle of a POV on the HID.
   */
  public BooleanEvent pov(int angle, EventLoop loop) {
    return pov(0, angle, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around this angle of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (e.g. right is 90,
   * upper-left is 315).
   *
   * @param pov index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around this angle of a POV on the HID.
   */
  public BooleanEvent pov(int pov, int angle, EventLoop loop) {
    synchronized (m_povCache) {
      var cache = m_povCache.computeIfAbsent(loop, k -> new HashMap<>());
      // angle can be -1, so use 3600 instead of 360
      return cache.computeIfAbsent(
          pov * 3600 + angle, k -> new BooleanEvent(loop, () -> getPOV(pov) == angle));
    }
  }

  /**
   * Constructs a BooleanEvent instance based around the 0 degree angle (up) of the default (index
   * 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the 0 degree angle of a POV on the HID.
   */
  public BooleanEvent povUp(EventLoop loop) {
    return pov(0, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the 45 degree angle (right up) of the default
   * (index 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the 45 degree angle of a POV on the HID.
   */
  public BooleanEvent povUpRight(EventLoop loop) {
    return pov(45, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the 90 degree angle (right) of the default
   * (index 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the 90 degree angle of a POV on the HID.
   */
  public BooleanEvent povRight(EventLoop loop) {
    return pov(90, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the 135 degree angle (right down) of the
   * default (index 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the 135 degree angle of a POV on the HID.
   */
  public BooleanEvent povDownRight(EventLoop loop) {
    return pov(135, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the 180 degree angle (down) of the default
   * (index 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the 180 degree angle of a POV on the HID.
   */
  public BooleanEvent povDown(EventLoop loop) {
    return pov(180, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the 225 degree angle (down left) of the default
   * (index 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the 225 degree angle of a POV on the HID.
   */
  public BooleanEvent povDownLeft(EventLoop loop) {
    return pov(225, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the 270 degree angle (left) of the default
   * (index 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the 270 degree angle of a POV on the HID.
   */
  public BooleanEvent povLeft(EventLoop loop) {
    return pov(270, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the 315 degree angle (left up) of the default
   * (index 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the 315 degree angle of a POV on the HID.
   */
  public BooleanEvent povUpLeft(EventLoop loop) {
    return pov(315, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the center (not pressed) of the default (index
   * 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the center of a POV on the HID.
   */
  public BooleanEvent povCenter(EventLoop loop) {
    return pov(-1, loop);
  }

  /**
   * Constructs an event instance that is true when the axis value is less than {@code threshold},
   * attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this event should return true.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the axis value is less than the provided threshold.
   */
  public BooleanEvent axisLessThan(int axis, double threshold, EventLoop loop) {
    synchronized (m_axisLessThanCache) {
      var cache = m_axisLessThanCache.computeIfAbsent(loop, k -> new HashMap<>());
      return cache.computeIfAbsent(
          Pair.of(axis, threshold),
          k -> new BooleanEvent(loop, () -> getRawAxis(axis) < threshold));
    }
  }

  /**
   * Constructs an event instance that is true when the axis value is greater than {@code
   * threshold}, attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this event should return true.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public BooleanEvent axisGreaterThan(int axis, double threshold, EventLoop loop) {
    synchronized (m_axisGreaterThanCache) {
      var cache = m_axisGreaterThanCache.computeIfAbsent(loop, k -> new HashMap<>());
      return cache.computeIfAbsent(
          Pair.of(axis, threshold),
          k -> new BooleanEvent(loop, () -> getRawAxis(axis) > threshold));
    }
  }

  /**
   * Get the number of axes for the HID.
   *
   * @return the number of axis for the current HID
   */
  public int getAxisCount() {
    return DriverStation.getStickAxisCount(m_port);
  }

  /**
   * For the current HID, return the number of POVs.
   *
   * @return the number of POVs for the current HID
   */
  public int getPOVCount() {
    return DriverStation.getStickPOVCount(m_port);
  }

  /**
   * For the current HID, return the number of buttons.
   *
   * @return the number of buttons for the current HID
   */
  public int getButtonCount() {
    return DriverStation.getStickButtonCount(m_port);
  }

  /**
   * Get if the HID is connected.
   *
   * @return true if the HID is connected
   */
  public boolean isConnected() {
    return DriverStation.isJoystickConnected(m_port);
  }

  /**
   * Get the type of the HID.
   *
   * @return the type of the HID.
   */
  public HIDType getType() {
    return HIDType.of(DriverStation.getJoystickType(m_port));
  }

  /**
   * Get the name of the HID.
   *
   * @return the name of the HID.
   */
  public String getName() {
    return DriverStation.getJoystickName(m_port);
  }

  /**
   * Get the axis type of the provided joystick axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return the axis type of the given joystick axis
   */
  public int getAxisType(int axis) {
    return DriverStation.getJoystickAxisType(m_port, axis);
  }

  /**
   * Get the port number of the HID.
   *
   * @return The port number of the HID.
   */
  public int getPort() {
    return m_port;
  }

  /**
   * Set a single HID output value for the HID.
   *
   * @param outputNumber The index of the output to set (1-32)
   * @param value The value to set the output to
   */
  public void setOutput(int outputNumber, boolean value) {
    m_outputs = (m_outputs & ~(1 << (outputNumber - 1))) | ((value ? 1 : 0) << (outputNumber - 1));
    DriverStationJNI.setJoystickOutputs((byte) m_port, m_outputs, m_leftRumble, m_rightRumble);
  }

  /**
   * Set all HID output values for the HID.
   *
   * @param value The 32 bit output value (1 bit for each output)
   */
  public void setOutputs(int value) {
    m_outputs = value;
    DriverStationJNI.setJoystickOutputs((byte) m_port, m_outputs, m_leftRumble, m_rightRumble);
  }

  /**
   * Set the rumble output for the HID. The DS currently supports 2 rumble values, left rumble and
   * right rumble.
   *
   * @param type Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
  public void setRumble(RumbleType type, double value) {
    value = MathUtil.clamp(value, 0, 1);
    int rumbleValue = (int) (value * 65535);
    switch (type) {
      case kLeftRumble -> this.m_leftRumble = rumbleValue;
      case kRightRumble -> this.m_rightRumble = rumbleValue;
      default -> {
        this.m_leftRumble = rumbleValue;
        this.m_rightRumble = rumbleValue;
      }
    }

    DriverStationJNI.setJoystickOutputs(
        (byte) this.m_port, this.m_outputs, this.m_leftRumble, this.m_rightRumble);
  }
}
