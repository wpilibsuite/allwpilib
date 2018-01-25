/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <support/deprecated.h>

#include "GenericHID.h"

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
  static constexpr int kDefaultXAxis = 0;
  static constexpr int kDefaultYAxis = 1;
  static constexpr int kDefaultZAxis = 2;
  static constexpr int kDefaultTwistAxis = 2;
  static constexpr int kDefaultThrottleAxis = 3;

  enum AxisType { kXAxis, kYAxis, kZAxis, kTwistAxis, kThrottleAxis };
  enum ButtonType { kTriggerButton, kTopButton };

  explicit Joystick(int port);
  virtual ~Joystick() = default;

  Joystick(const Joystick&) = delete;
  Joystick& operator=(const Joystick&) = delete;

  void SetXChannel(int channel);
  void SetYChannel(int channel);
  void SetZChannel(int channel);
  void SetTwistChannel(int channel);
  void SetThrottleChannel(int channel);

  WPI_DEPRECATED("Use the more specific axis channel setter functions.")
  void SetAxisChannel(AxisType axis, int channel);

  int GetXChannel() const;
  int GetYChannel() const;
  int GetZChannel() const;
  int GetTwistChannel() const;
  int GetThrottleChannel() const;

  WPI_DEPRECATED("Use the more specific axis channel getter functions.")
  int GetAxisChannel(AxisType axis) const;

  double GetX(JoystickHand hand = kRightHand) const override;
  double GetY(JoystickHand hand = kRightHand) const override;
  double GetZ() const;
  double GetTwist() const;
  double GetThrottle() const;

  WPI_DEPRECATED("Use the more specific axis channel getter functions.")
  double GetAxis(AxisType axis) const;

  bool GetTrigger() const;
  bool GetTriggerPressed();
  bool GetTriggerReleased();

  bool GetTop() const;
  bool GetTopPressed();
  bool GetTopReleased();

  WPI_DEPRECATED("Use Joystick instances instead.")
  static Joystick* GetStickForPort(int port);

  WPI_DEPRECATED("Use the more specific button getter functions.")
  bool GetButton(ButtonType button) const;

  double GetMagnitude() const;
  double GetDirectionRadians() const;
  double GetDirectionDegrees() const;

 private:
  enum Axis { kX, kY, kZ, kTwist, kThrottle, kNumAxes };
  enum Button { kTrigger = 1, kTop = 2 };

  std::array<int, Axis::kNumAxes> m_axes;
};

}  // namespace frc
