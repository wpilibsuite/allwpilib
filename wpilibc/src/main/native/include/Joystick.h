/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <vector>

#include "ErrorBase.h"
#include "JoystickBase.h"

namespace frc {

class DriverStation;

/**
 * Handle input from standard Joysticks connected to the Driver Station.
 * This class handles standard input that comes from the Driver Station. Each
 * time a value is requested the most recent value is returned. There is a
 * single class instance for each joystick and the mapping of ports to hardware
 * buttons depends on the code in the Driver Station.
 */
class Joystick : public JoystickBase, public ErrorBase {
 public:
  static const int kDefaultXAxis = 0;
  static const int kDefaultYAxis = 1;
  static const int kDefaultZAxis = 2;
  static const int kDefaultTwistAxis = 2;
  static const int kDefaultThrottleAxis = 3;

  typedef enum {
    kXAxis,
    kYAxis,
    kZAxis,
    kTwistAxis,
    kThrottleAxis,
    kNumAxisTypes
  } AxisType;

  static const int kDefaultTriggerButton = 1;
  static const int kDefaultTopButton = 2;

  typedef enum { kTriggerButton, kTopButton, kNumButtonTypes } ButtonType;

  explicit Joystick(int port);
  Joystick(int port, int numAxisTypes, int numButtonTypes);
  virtual ~Joystick() = default;

  Joystick(const Joystick&) = delete;
  Joystick& operator=(const Joystick&) = delete;

  int GetAxisChannel(AxisType axis) const;
  void SetAxisChannel(AxisType axis, int channel);

  double GetX(JoystickHand hand = kRightHand) const override;
  double GetY(JoystickHand hand = kRightHand) const override;
  double GetZ(JoystickHand hand = kRightHand) const override;
  double GetTwist() const override;
  double GetThrottle() const override;
  virtual double GetAxis(AxisType axis) const;

  bool GetTrigger(JoystickHand hand = kRightHand) const override;
  bool GetTop(JoystickHand hand = kRightHand) const override;
  bool GetButton(ButtonType button) const;
  static Joystick* GetStickForPort(int port);

  virtual double GetMagnitude() const;
  virtual double GetDirectionRadians() const;
  virtual double GetDirectionDegrees() const;

  int GetAxisType(int axis) const;

  int GetAxisCount() const;
  int GetButtonCount() const;

 private:
  DriverStation& m_ds;
  std::vector<int> m_axes;
  std::vector<int> m_buttons;
};

}  // namespace frc
