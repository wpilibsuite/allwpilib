/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <wpi/deprecated.h>

#include "frc/GenericHID.h"

namespace frc {

/**
 * Handle input from standard Joysticks connected to the Driver Station.
 *
 * This class handles standard input that comes from the Driver Station. Each
 * time a value is requested the most recent value is returned. There is a
 * single class instance for each joystick and the mapping of ports to hardware
 * buttons depends on the code in the Driver Station.
 */
class Joystick : public GenericHID {
 public:
  static constexpr int kDefaultXChannel = 0;
  static constexpr int kDefaultYChannel = 1;
  static constexpr int kDefaultZChannel = 2;
  static constexpr int kDefaultTwistChannel = 2;
  static constexpr int kDefaultThrottleChannel = 3;

  WPI_DEPRECATED("Use kDefaultXChannel instead.")
  static constexpr int kDefaultXAxis = 0;
  WPI_DEPRECATED("Use kDefaultYChannel instead.")
  static constexpr int kDefaultYAxis = 1;
  WPI_DEPRECATED("Use kDefaultZChannel instead.")
  static constexpr int kDefaultZAxis = 2;
  WPI_DEPRECATED("Use kDefaultTwistChannel instead.")
  static constexpr int kDefaultTwistAxis = 2;
  WPI_DEPRECATED("Use kDefaultThrottleChannel instead.")
  static constexpr int kDefaultThrottleAxis = 3;

  enum AxisType { kXAxis, kYAxis, kZAxis, kTwistAxis, kThrottleAxis };
  enum ButtonType { kTriggerButton, kTopButton };

  /**
   * Construct an instance of a joystick.
   *
   * The joystick index is the USB port on the Driver Station.
   *
   * @param port The port on the Driver Station that the joystick is plugged
   *             into (0-5).
   */
  explicit Joystick(int port);

  virtual ~Joystick() = default;

  Joystick(Joystick&&) = default;
  Joystick& operator=(Joystick&&) = default;

  /**
   * Set the channel associated with the X axis.
   *
   * @param channel The channel to set the axis to.
   */
  void SetXChannel(int channel);

  /**
   * Set the channel associated with the Y axis.
   *
   * @param axis    The axis to set the channel for.
   * @param channel The channel to set the axis to.
   */
  void SetYChannel(int channel);

  /**
   * Set the channel associated with the Z axis.
   *
   * @param axis    The axis to set the channel for.
   * @param channel The channel to set the axis to.
   */
  void SetZChannel(int channel);

  /**
   * Set the channel associated with the twist axis.
   *
   * @param axis    The axis to set the channel for.
   * @param channel The channel to set the axis to.
   */
  void SetTwistChannel(int channel);

  /**
   * Set the channel associated with the throttle axis.
   *
   * @param axis    The axis to set the channel for.
   * @param channel The channel to set the axis to.
   */
  void SetThrottleChannel(int channel);

  /**
   * Set the channel associated with a specified axis.
   *
   * @param axis    The axis to set the channel for.
   * @param channel The channel to set the axis to.
   */
  WPI_DEPRECATED("Use the more specific axis channel setter functions.")
  void SetAxisChannel(AxisType axis, int channel);

  /**
   * Get the channel currently associated with the X axis.
   *
   * @return The channel for the axis.
   */
  int GetXChannel() const;

  /**
   * Get the channel currently associated with the Y axis.
   *
   * @return The channel for the axis.
   */
  int GetYChannel() const;

  /**
   * Get the channel currently associated with the Z axis.
   *
   * @return The channel for the axis.
   */
  int GetZChannel() const;

  /**
   * Get the channel currently associated with the twist axis.
   *
   * @return The channel for the axis.
   */
  int GetTwistChannel() const;

  /**
   * Get the channel currently associated with the throttle axis.
   *
   * @return The channel for the axis.
   */
  int GetThrottleChannel() const;

  /**
   * Get the X value of the joystick.
   *
   * This depends on the mapping of the joystick connected to the current port.
   *
   * @param hand This parameter is ignored for the Joystick class and is only
   *             here to complete the GenericHID interface.
   */
  double GetX(JoystickHand hand = kRightHand) const override;

  /**
   * Get the Y value of the joystick.
   *
   * This depends on the mapping of the joystick connected to the current port.
   *
   * @param hand This parameter is ignored for the Joystick class and is only
   *             here to complete the GenericHID interface.
   */
  double GetY(JoystickHand hand = kRightHand) const override;

  /**
   * Get the Z value of the current joystick.
   *
   * This depends on the mapping of the joystick connected to the current port.
   */
  double GetZ() const;

  /**
   * Get the twist value of the current joystick.
   *
   * This depends on the mapping of the joystick connected to the current port.
   */
  double GetTwist() const;

  /**
   * Get the throttle value of the current joystick.
   *
   * This depends on the mapping of the joystick connected to the current port.
   */
  double GetThrottle() const;

  /**
   * For the current joystick, return the axis determined by the argument.
   *
   * This is for cases where the joystick axis is returned programatically,
   * otherwise one of the previous functions would be preferable (for example
   * GetX()).
   *
   * @param axis The axis to read.
   * @return The value of the axis.
   */
  WPI_DEPRECATED("Use the more specific axis channel getter functions.")
  double GetAxis(AxisType axis) const;

  /**
   * Read the state of the trigger on the joystick.
   *
   * Look up which button has been assigned to the trigger and read its state.
   *
   * @return The state of the trigger.
   */
  bool GetTrigger() const;

  /**
   * Whether the trigger was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetTriggerPressed();

  /**
   * Whether the trigger was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetTriggerReleased();

  /**
   * Read the state of the top button on the joystick.
   *
   * Look up which button has been assigned to the top and read its state.
   *
   * @return The state of the top button.
   */
  bool GetTop() const;

  /**
   * Whether the top button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetTopPressed();

  /**
   * Whether the top button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetTopReleased();

  WPI_DEPRECATED("Use Joystick instances instead.")
  static Joystick* GetStickForPort(int port);

  /**
   * Get buttons based on an enumerated type.
   *
   * The button type will be looked up in the list of buttons and then read.
   *
   * @param button The type of button to read.
   * @return The state of the button.
   */
  WPI_DEPRECATED("Use the more specific button getter functions.")
  bool GetButton(ButtonType button) const;

  /**
   * Get the magnitude of the direction vector formed by the joystick's
   * current position relative to its origin.
   *
   * @return The magnitude of the direction vector
   */
  double GetMagnitude() const;

  /**
   * Get the direction of the vector formed by the joystick and its origin
   * in radians.
   *
   * @return The direction of the vector in radians
   */
  double GetDirectionRadians() const;

  /**
   * Get the direction of the vector formed by the joystick and its origin
   * in degrees.
   *
   * @return The direction of the vector in degrees
   */
  double GetDirectionDegrees() const;

 private:
  enum Axis { kX, kY, kZ, kTwist, kThrottle, kNumAxes };
  enum Button { kTrigger = 1, kTop = 2 };

  std::array<int, Axis::kNumAxes> m_axes;
};

}  // namespace frc
