/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/XboxController.h"
#include "frc/simulation/GenericHIDSim.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated Xbox 360 or Xbox One controller.
 */
class XboxControllerSim : public GenericHIDSim {
 public:
  /**
   * Constructs from a XboxController object.
   *
   * @param joystick controller to simulate
   */
  explicit XboxControllerSim(const XboxController& joystick)
      : GenericHIDSim{joystick} {
    SetAxisCount(6);
    SetButtonCount(10);
  }

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  explicit XboxControllerSim(int port) : GenericHIDSim{port} {
    SetAxisCount(6);
    SetButtonCount(10);
  }

  void SetX(GenericHID::JoystickHand hand, double value) {
    if (hand == GenericHID::kLeftHand) {
      SetRawAxis(static_cast<int>(XboxController::Axis::kLeftX), value);
    } else {
      SetRawAxis(static_cast<int>(XboxController::Axis::kRightX), value);
    }
  }

  void SetY(GenericHID::JoystickHand hand, double value) {
    if (hand == GenericHID::kLeftHand) {
      SetRawAxis(static_cast<int>(XboxController::Axis::kLeftY), value);
    } else {
      SetRawAxis(static_cast<int>(XboxController::Axis::kRightY), value);
    }
  }

  void SetTriggerAxis(GenericHID::JoystickHand hand, double value) {
    if (hand == GenericHID::kLeftHand) {
      SetRawAxis(static_cast<int>(XboxController::Axis::kLeftTrigger), value);
    } else {
      SetRawAxis(static_cast<int>(XboxController::Axis::kRightTrigger), value);
    }
  }

  void SetBumper(GenericHID::JoystickHand hand, bool state) {
    if (hand == GenericHID::kLeftHand) {
      SetRawButton(static_cast<int>(XboxController::Button::kBumperLeft),
                   state);
    } else {
      SetRawButton(static_cast<int>(XboxController::Button::kBumperRight),
                   state);
    }
  }

  void SetStickButton(GenericHID::JoystickHand hand, bool state) {
    if (hand == GenericHID::kLeftHand) {
      SetRawButton(static_cast<int>(XboxController::Button::kStickLeft), state);
    } else {
      SetRawButton(static_cast<int>(XboxController::Button::kStickRight),
                   state);
    }
  }

  void SetAButton(bool state) {
    SetRawButton(static_cast<int>(XboxController::Button::kA), state);
  }

  void SetBButton(bool state) {
    SetRawButton(static_cast<int>(XboxController::Button::kB), state);
  }

  void SetXButton(bool state) {
    SetRawButton(static_cast<int>(XboxController::Button::kX), state);
  }

  void SetYButton(bool state) {
    SetRawButton(static_cast<int>(XboxController::Button::kY), state);
  }

  void SetBackButton(bool state) {
    SetRawButton(static_cast<int>(XboxController::Button::kBack), state);
  }

  void SetStartButton(bool state) {
    SetRawButton(static_cast<int>(XboxController::Button::kStart), state);
  }
};

}  // namespace sim
}  // namespace frc
