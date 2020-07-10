/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/Joystick.h"
#include "frc/simulation/GenericHIDSim.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated joystick.
 */
class JoystickSim : public GenericHIDSim {
 public:
  /**
   * Constructs from a Joystick object.
   *
   * @param joystick joystick to simulate
   */
  explicit JoystickSim(const Joystick& joystick)
      : GenericHIDSim{joystick}, m_joystick{&joystick} {
    // default to a reasonable joystick configuration
    SetAxisCount(5);
    SetButtonCount(12);
    SetPOVCount(1);
  }

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  explicit JoystickSim(int port) : GenericHIDSim{port} {
    // default to a reasonable joystick configuration
    SetAxisCount(5);
    SetButtonCount(12);
    SetPOVCount(1);
  }

  void SetX(double value) {
    SetRawAxis(
        m_joystick ? m_joystick->GetXChannel() : Joystick::kDefaultXChannel,
        value);
  }

  void SetY(double value) {
    SetRawAxis(
        m_joystick ? m_joystick->GetYChannel() : Joystick::kDefaultYChannel,
        value);
  }

  void SetZ(double value) {
    SetRawAxis(
        m_joystick ? m_joystick->GetZChannel() : Joystick::kDefaultZChannel,
        value);
  }

  void SetTwist(double value) {
    SetRawAxis(m_joystick ? m_joystick->GetTwistChannel()
                          : Joystick::kDefaultTwistChannel,
               value);
  }

  void SetThrottle(double value) {
    SetRawAxis(m_joystick ? m_joystick->GetThrottleChannel()
                          : Joystick::kDefaultThrottleChannel,
               value);
  }

  void SetTrigger(bool state) { SetRawButton(1, state); }

  void SetTop(bool state) { SetRawButton(2, state); }

 private:
  const Joystick* m_joystick = nullptr;
};

}  // namespace sim
}  // namespace frc
