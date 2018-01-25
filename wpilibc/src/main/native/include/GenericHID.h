/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <string>

#include "ErrorBase.h"

namespace frc {

class DriverStation;

/**
 * GenericHID Interface.
 */
class GenericHID : public ErrorBase {
 public:
  enum RumbleType { kLeftRumble, kRightRumble };

  enum HIDType {
    kUnknown = -1,
    kXInputUnknown = 0,
    kXInputGamepad = 1,
    kXInputWheel = 2,
    kXInputArcadeStick = 3,
    kXInputFlightStick = 4,
    kXInputDancePad = 5,
    kXInputGuitar = 6,
    kXInputGuitar2 = 7,
    kXInputDrumKit = 8,
    kXInputGuitar3 = 11,
    kXInputArcadePad = 19,
    kHIDJoystick = 20,
    kHIDGamepad = 21,
    kHIDDriving = 22,
    kHIDFlight = 23,
    kHID1stPerson = 24
  };

  enum JoystickHand { kLeftHand = 0, kRightHand = 1 };

  explicit GenericHID(int port);
  virtual ~GenericHID() = default;

  virtual double GetX(JoystickHand hand = kRightHand) const = 0;
  virtual double GetY(JoystickHand hand = kRightHand) const = 0;

  bool GetRawButton(int button) const;
  bool GetRawButtonPressed(int button);
  bool GetRawButtonReleased(int button);

  double GetRawAxis(int axis) const;
  int GetPOV(int pov = 0) const;

  int GetAxisCount() const;
  int GetPOVCount() const;
  int GetButtonCount() const;

  GenericHID::HIDType GetType() const;
  std::string GetName() const;
  int GetAxisType(int axis) const;

  int GetPort() const;

  void SetOutput(int outputNumber, bool value);
  void SetOutputs(int value);
  void SetRumble(RumbleType type, double value);

 private:
  DriverStation& m_ds;
  int m_port;
  int m_outputs = 0;
  uint16_t m_leftRumble = 0;
  uint16_t m_rightRumble = 0;
};

}  // namespace frc
