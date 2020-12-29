// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

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
class Trigger : public Sendable, public SendableHelper<Trigger> {
 public:
  Trigger() = default;
  ~Trigger() override = default;

  Trigger(const Trigger& rhs);
  Trigger& operator=(const Trigger& rhs);
  Trigger(Trigger&& rhs);
  Trigger& operator=(Trigger&& rhs);

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
