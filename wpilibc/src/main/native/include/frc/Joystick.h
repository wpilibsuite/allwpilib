// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <units/angle.h>

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
  /// Default X axis channel.
  static constexpr int kDefaultXChannel = 0;
  /// Default Y axis channel.
  static constexpr int kDefaultYChannel = 1;
  /// Default Z axis channel.
  static constexpr int kDefaultZChannel = 2;
  /// Default twist axis channel.
  static constexpr int kDefaultTwistChannel = 2;
  /// Default throttle axis channel.
  static constexpr int kDefaultThrottleChannel = 3;

  /**
   * Represents an analog axis on a joystick.
   */
  enum AxisType {
    /// X axis.
    kXAxis,
    /// Y axis.
    kYAxis,
    /// Z axis.
    kZAxis,
    /// Twist axis.
    kTwistAxis,
    /// Throttle axis.
    kThrottleAxis
  };

  /**
   * Represents a digital button on a joystick.
   */
  enum ButtonType {
    /// kTrigger.
    kTriggerButton,
    /// kTop.
    kTopButton
  };

  /**
   * Construct an instance of a joystick.
   *
   * The joystick index is the USB port on the Driver Station.
   *
   * @param port The port on the Driver Station that the joystick is plugged
   *             into (0-5).
   */
  explicit Joystick(int port);

  ~Joystick() override = default;

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
   * @param channel The channel to set the axis to.
   */
  void SetYChannel(int channel);

  /**
   * Set the channel associated with the Z axis.
   *
   * @param channel The channel to set the axis to.
   */
  void SetZChannel(int channel);

  /**
   * Set the channel associated with the twist axis.
   *
   * @param channel The channel to set the axis to.
   */
  void SetTwistChannel(int channel);

  /**
   * Set the channel associated with the throttle axis.
   *
   * @param channel The channel to set the axis to.
   */
  void SetThrottleChannel(int channel);

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
   * Get the X value of the current joystick.
   *
   * This depends on the mapping of the joystick connected to the current port.
   */
  double GetX() const;

  /**
   * Get the Y value of the current joystick.
   *
   * This depends on the mapping of the joystick connected to the current port.
   */
  double GetY() const;

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
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the trigger button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Trigger(EventLoop* loop) const;

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

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the top button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Top(EventLoop* loop) const;

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
   * @deprecated Use GetDirection() instead.
   */
  [[deprecated("Use GetDirection() instead.")]]
  double GetDirectionRadians() const;

  /**
   * Get the direction of the vector formed by the joystick and its origin
   * in degrees.
   *
   * @return The direction of the vector in degrees
   * @deprecated Use GetDirection() instead.
   */
  [[deprecated("Use GetDirection() instead.")]]
  double GetDirectionDegrees() const;

  /**
   * Get the direction of the vector formed by the joystick and its origin.
   *
   * @return The direction of the vector.
   */
  units::radian_t GetDirection() const;

 private:
  enum Axis { kX, kY, kZ, kTwist, kThrottle, kNumAxes };
  enum Button { kTrigger = 1, kTop = 2 };

  std::array<int, Axis::kNumAxes> m_axes;
};

}  // namespace frc
