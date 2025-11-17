// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>

#include "wpi/driverstation/DriverStation.hpp"

namespace wpi {

class BooleanEvent;
class EventLoop;

/**
 * Handle input from standard HID devices connected to the Driver Station.
 *
 * <p>This class handles standard input that comes from the Driver Station. Each
 * time a value is requested the most recent value is returned. There is a
 * single class instance for each device and the mapping of ports to hardware
 * buttons depends on the code in the Driver Station.
 */
class GenericHID {
 public:
  /**
   * Represents a rumble output on the Joystick.
   */
  enum RumbleType {
    /// Left rumble motor.
    kLeftRumble,
    /// Right rumble motor.
    kRightRumble,
    /// Left trigger rumble motor.
    kLeftTriggerRumble,
    /// Right trigger rumble motor.
    kRightTriggerRumble,
  };

  /**
   * Represents the various outputs that a HID may support.
   */
  enum SupportedOutputs {
    /// No outputs supported.
    kNone = 0x0,
    /// Mono LED support.
    kMonoLed = 0x1,
    /// RGB LED support.
    kRgbLed = 0x2,
    /// Player LED support.
    kPlayerLed = 0x4,
    /// Rumble support.
    kRumble = 0x8,
    /// Trigger rumble support.
    kTriggerRumble = 0x10,
  };

  /**
   * USB HID interface type.
   */
  enum HIDType {
    /// Unknown.
    kUnknown = 0,
    /// Standard HID device.
    kStandard,
    /// Xbox 360 controller.
    kXbox360,
    /// Xbox One controller.
    kXboxOne,
    /// PS3 controller.
    kPS3,
    /// PS4 controller.
    kPS4,
    /// PS5 controller.
    kPS5,
    /// Nintendo Switch Pro controller.
    kSwitchPro,
    /// Nintendo Switch Joycon Left controller.
    kSwitchJoyconLeft,
    /// Nintendo Switch Joycon Right controller.
    kSwitchJoyconRight,
    /// Nintendo Switch Joycon controller pair.
    kSwitchJoyconPair
  };

  explicit GenericHID(int port);
  virtual ~GenericHID() = default;

  GenericHID(GenericHID&&) = default;
  GenericHID& operator=(GenericHID&&) = default;

  /**
   * Get the button value (starting at button 1).
   *
   * The buttons are returned in a single 16 bit value with one bit representing
   * the state of each button. The appropriate button is returned as a boolean
   * value.
   *
   * This method returns true if the button is being held down at the time
   * that this method is being called.
   *
   * @param button The button number to be read (starting at 1)
   * @return The state of the button.
   */
  bool GetRawButton(int button) const;

  /**
   * Whether the button was pressed since the last check. %Button indexes begin
   * at 1.
   *
   * This method returns true if the button went from not pressed to held down
   * since the last time this method was called. This is useful if you only
   * want to call a function once when you press the button.
   *
   * @param button The button index, beginning at 0.
   * @return Whether the button was pressed since the last check.
   */
  bool GetRawButtonPressed(int button);

  /**
   * Whether the button was released since the last check. %Button indexes begin
   * at 1.
   *
   * This method returns true if the button went from held down to not pressed
   * since the last time this method was called. This is useful if you only
   * want to call a function once when you release the button.
   *
   * @param button The button index, beginning at 0.
   * @return Whether the button was released since the last check.
   */
  bool GetRawButtonReleased(int button);

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached
   * to the given loop.
   */
  BooleanEvent Button(int button, EventLoop* loop) const;

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return The value of the axis.
   */
  double GetRawAxis(int axis) const;

  /**
   * Get the angle of a POV on the HID.
   *
   * @param pov The index of the POV to read (starting at 0)
   * @return the angle of the POV.
   */
  DriverStation::POVDirection GetPOV(int pov = 0) const;

  /**
   * Constructs a BooleanEvent instance based around this angle of a POV on the
   * HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @param angle POV angle.
   * @return a BooleanEvent instance based around this angle of a POV on the
   * HID.
   */
  BooleanEvent POV(DriverStation::POVDirection angle, EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around this angle of a POV on the
   * HID.
   *
   * @param loop the event loop instance to attach the event to.
   * @param pov   index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle.
   * @return a BooleanEvent instance based around this angle of a POV on the
   * HID.
   */
  BooleanEvent POV(int pov, DriverStation::POVDirection angle,
                   EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the up direction of
   * the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the up direction of a POV on
   * the HID.
   */
  BooleanEvent POVUp(EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the up right direction
   * of the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the up right direction of a
   * POV on the HID.
   */
  BooleanEvent POVUpRight(EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the right direction
   * of the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the right direction of a POV
   * on the HID.
   */
  BooleanEvent POVRight(EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the down right direction
   * of the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the down right direction of a
   * POV on the HID.
   */
  BooleanEvent POVDownRight(EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the down direction
   * of the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the down direction of a POV
   * on the HID.
   */
  BooleanEvent POVDown(EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the down left direction
   * of the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the down left direction of a
   * POV on the HID.
   */
  BooleanEvent POVDownLeft(EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the left direction
   * of the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the left direction of a POV
   * on the HID.
   */
  BooleanEvent POVLeft(EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the up left direction
   * of the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the up left direction of a POV
   * on the HID.
   */
  BooleanEvent POVUpLeft(EventLoop* loop) const;

  /**
   * Constructs a BooleanEvent instance based around the center (not pressed) of
   * the default (index 0) POV on the HID.
   *
   * @return a BooleanEvent instance based around the center of a POV on the
   * HID.
   */
  BooleanEvent POVCenter(EventLoop* loop) const;

  /**
   * Constructs an event instance that is true when the axis value is less than
   * threshold
   *
   * @param axis The axis to read, starting at 0.
   * @param threshold The value below which this trigger should return true.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the axis value is less than the
   * provided threshold.
   */
  BooleanEvent AxisLessThan(int axis, double threshold, EventLoop* loop) const;

  /**
   * Constructs an event instance that is true when the axis value is greater
   * than threshold
   *
   * @param axis The axis to read, starting at 0.
   * @param threshold The value above which this trigger should return true.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the axis value is greater than
   * the provided threshold.
   */
  BooleanEvent AxisGreaterThan(int axis, double threshold,
                               EventLoop* loop) const;

  int GetAxesMaximumIndex() const;

  /**
   * Get the number of axes for the HID.
   *
   * @return the number of axis for the current HID
   */
  int GetAxesAvailable() const;

  int GetPOVsMaximumIndex() const;

  /**
   * Get the number of POVs for the HID.
   *
   * @return the number of POVs for the current HID
   */
  int GetPOVsAvailable() const;

  int GetButtonsMaximumIndex() const;

  /**
   * Get the number of buttons for the HID.
   *
   * @return the number of buttons on the current HID
   */
  uint64_t GetButtonsAvailable() const;

  /**
   * Get if the HID is connected.
   *
   * @return true if the HID is connected
   */
  bool IsConnected() const;

  /**
   * Get the type of the HID.
   *
   * @return the type of the HID.
   */
  GenericHID::HIDType GetGamepadType() const;

  /**
   * Get the supported outputs of the HID.
   *
   * @return the supported outputs of the HID.
   */
  GenericHID::SupportedOutputs GetSupportedOutputs() const;

  /**
   * Get the name of the HID.
   *
   * @return the name of the HID.
   */
  std::string GetName() const;

  /**
   * Get the port number of the HID.
   *
   * @return The port number of the HID.
   */
  int GetPort() const;

  /**
   * Set leds on the controller. If only mono is supported, the system will use
   * the highest value passed in.
   *
   * @param r Red value from 0-255
   * @param g Green value from 0-255
   * @param b Blue value from 0-255
   */
  void SetLeds(int r, int g, int b);

  /**
   * Set the rumble output for the HID.
   *
   * The DS currently supports 2 rumble values, left rumble and right rumble.
   *
   * @param type  Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
  void SetRumble(RumbleType type, double value);

 private:
  int m_port;
  uint16_t m_leftRumble = 0;
  uint16_t m_rightRumble = 0;
  uint16_t m_leftTriggerRumble = 0;
  uint16_t m_rightTriggerRumble = 0;
};

}  // namespace wpi
