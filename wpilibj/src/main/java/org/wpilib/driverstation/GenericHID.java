// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import java.util.HashMap;
import java.util.Map;
import org.wpilib.driverstation.DriverStation.POVDirection;
import org.wpilib.event.BooleanEvent;
import org.wpilib.event.EventLoop;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.math.util.Pair;

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
    /** Left trigger rumble motor. */
    kLeftTriggerRumble,
    /** Right trigger rumble motor. */
    kRightTriggerRumble,
  }

  /** USB HID interface type. */
  public enum HIDType {
    /** Unknown. */
    kUnknown(0),
    /** Standard. */
    kStandard(1),
    /** Xbox 360. */
    kXbox360(2),
    /** Xbox One. */
    kXboxOne(3),
    /** PS3. */
    kPS3(4),
    /** PS4. */
    kPS4(5),
    /** PS5. */
    kPS5(6),
    /** Switch Pro. */
    kSwitchPro(7),
    /** Switch Joycon Left. */
    kSwitchJoyconLeft(8),
    /** Switch Joycon Right. */
    kSwitchJoyconRight(9),
    /** Switch Joycon Pair. */
    kSwitchJoyconPair(10);

    /** HIDType value. */
    public final int value;

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
  private int m_leftRumble;
  private int m_rightRumble;
  private int m_leftTriggerRumble;
  private int m_rightTriggerRumble;
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
   * @param button The button index, beginning at 0.
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
   * @param button The button index, beginning at 0.
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
   * Get the angle of a POV on the HID.
   *
   * @param pov The index of the POV to read (starting at 0). Defaults to 0.
   * @return the angle of the POV.
   */
  public POVDirection getPOV(int pov) {
    return DriverStation.getStickPOV(m_port, pov);
  }

  /**
   * Get the angle of the default POV (index 0) on the HID.
   *
   * @return the angle of the POV.
   */
  public POVDirection getPOV() {
    return getPOV(0);
  }

  /**
   * Constructs a BooleanEvent instance based around this angle of a POV on the HID.
   *
   * @param angle POV angle.
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around this angle of a POV on the HID.
   */
  public BooleanEvent pov(POVDirection angle, EventLoop loop) {
    return pov(0, angle, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around this angle of a POV on the HID.
   *
   * @param pov index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle.
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around this angle of a POV on the HID.
   */
  public BooleanEvent pov(int pov, POVDirection angle, EventLoop loop) {
    synchronized (m_povCache) {
      var cache = m_povCache.computeIfAbsent(loop, k -> new HashMap<>());
      // angle.value is a 4 bit bitfield
      return cache.computeIfAbsent(
          pov * 16 + angle.value, k -> new BooleanEvent(loop, () -> getPOV(pov) == angle));
    }
  }

  /**
   * Constructs a BooleanEvent instance based around the up direction of the default (index 0) POV
   * on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the up direction a POV on the HID.
   */
  public BooleanEvent povUp(EventLoop loop) {
    return pov(POVDirection.Up, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the up right direction of the default (index 0)
   * POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the up right direction of a POV on the HID.
   */
  public BooleanEvent povUpRight(EventLoop loop) {
    return pov(POVDirection.UpRight, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the right direction of the default (index 0)
   * POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the right direction of a POV on the HID.
   */
  public BooleanEvent povRight(EventLoop loop) {
    return pov(POVDirection.Right, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the down right direction of the default (index
   * 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the down right direction of a POV on the HID.
   */
  public BooleanEvent povDownRight(EventLoop loop) {
    return pov(POVDirection.DownRight, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the down direction of the default (index 0) POV
   * on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the down direction of a POV on the HID.
   */
  public BooleanEvent povDown(EventLoop loop) {
    return pov(POVDirection.Down, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the down left direction of the default (index
   * 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the down left direction of a POV on the HID.
   */
  public BooleanEvent povDownLeft(EventLoop loop) {
    return pov(POVDirection.DownLeft, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the left direction of the default (index 0) POV
   * on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the left direction of a POV on the HID.
   */
  public BooleanEvent povLeft(EventLoop loop) {
    return pov(POVDirection.Left, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the up left direction of the default (index 0)
   * POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the up left direction of a POV on the HID.
   */
  public BooleanEvent povUpLeft(EventLoop loop) {
    return pov(POVDirection.UpLeft, loop);
  }

  /**
   * Constructs a BooleanEvent instance based around the center (not pressed) of the default (index
   * 0) POV on the HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a BooleanEvent instance based around the center of a POV on the HID.
   */
  public BooleanEvent povCenter(EventLoop loop) {
    return pov(POVDirection.Center, loop);
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
   * Get the maximum axis index for the joystick.
   *
   * @return the maximum axis index for the joystick
   */
  public int getAxesMaximumIndex() {
    return DriverStation.getStickAxesMaximumIndex(m_port);
  }

  /**
   * Get the number of axes for the HID.
   *
   * @return the number of axis for the current HID
   */
  public int getAxesAvailable() {
    return DriverStation.getStickAxesAvailable(m_port);
  }

  /**
   * Get the maximum POV index for the joystick.
   *
   * @return the maximum POV index for the joystick
   */
  public int getPOVsMaximumIndex() {
    return DriverStation.getStickPOVsMaximumIndex(m_port);
  }

  /**
   * For the current HID, return the number of POVs.
   *
   * @return the number of POVs for the current HID
   */
  public int getPOVsAvailable() {
    return DriverStation.getStickPOVsAvailable(m_port);
  }

  /**
   * Get the maximum button index for the joystick.
   *
   * @return the maximum button index for the joystick
   */
  public int getButtonsMaximumIndex() {
    return DriverStation.getStickButtonsMaximumIndex(m_port);
  }

  /**
   * For the current HID, return the number of buttons.
   *
   * @return the number of buttons for the current HID
   */
  public long getButtonsAvailable() {
    return DriverStation.getStickButtonsAvailable(m_port);
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
  public HIDType getGamepadType() {
    return HIDType.of(DriverStation.getJoystickGamepadType(m_port));
  }

  /**
   * Get the supported outputs for the HID.
   *
   * @return the supported outputs for the HID.
   */
  public int getSupportedOutputs() {
    return DriverStation.getJoystickSupportedOutputs(m_port);
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
   * Get the port number of the HID.
   *
   * @return The port number of the HID.
   */
  public int getPort() {
    return m_port;
  }

  /**
   * Set leds on the controller. If only mono is supported, the system will use the highest value
   * passed in.
   *
   * @param r Red value from 0-255
   * @param g Green value from 0-255
   * @param b Blue value from 0-255
   */
  public void setLeds(int r, int g, int b) {
    int value = ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
    DriverStationJNI.setJoystickLeds((byte) m_port, value);
  }

  /**
   * Set the rumble output for the HID. The DS currently supports 2 rumble values, left rumble and
   * right rumble.
   *
   * @param type Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
  public void setRumble(RumbleType type, double value) {
    value = Math.clamp(value, 0, 1);
    int rumbleValue = (int) (value * 65535);
    switch (type) {
      case kLeftRumble:
        this.m_leftRumble = rumbleValue;
        break;
      case kRightRumble:
        this.m_rightRumble = rumbleValue;
        break;
      case kLeftTriggerRumble:
        this.m_leftTriggerRumble = rumbleValue;
        break;
      case kRightTriggerRumble:
        this.m_rightTriggerRumble = rumbleValue;
        break;
      default:
        break;
    }

    DriverStationJNI.setJoystickRumble(
        (byte) this.m_port,
        this.m_leftRumble,
        this.m_rightRumble,
        this.m_leftTriggerRumble,
        this.m_rightTriggerRumble);
  }
}
