/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>

#include "frc/smartdashboard/SendableBase.h"

namespace frc {

class Command;

/**
 * This class provides an easy way to link commands to inputs.
 *
 * It is very easy to link a polled input to a command. For instance, you could
 * link the trigger button of a joystick to a "score" command or an encoder
 * reaching a particular value.
 *
 * It is encouraged that teams write a subclass of Trigger if they want to have
 * something unusual (for instance, if they want to react to the user holding
 * a button while the robot is reading a certain sensor input). For this, they
 * only have to write the {@link Trigger#Get()} method to get the full
 * functionality of the Trigger class.
 */
class Trigger : public SendableBase {
 public:
  Trigger() = default;
  ~Trigger() override = default;

  Trigger(Trigger&&) = default;
  Trigger& operator=(Trigger&&) = default;

  bool Grab();
  virtual bool Get() = 0;
  void WhenActive(Command* command);
  void WhileActive(Command* command);
  void WhenInactive(Command* command);
  void CancelWhenActive(Command* command);
  void ToggleWhenActive(Command* command);

  void InitSendable(SendableBuilder& builder) override;

 private:
  std::atomic_bool m_sendablePressed{false};
};

}  // namespace frc
