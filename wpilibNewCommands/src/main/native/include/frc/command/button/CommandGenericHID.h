// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/GenericHID.h>

#include "Trigger.h"
#include "frc/command/CommandScheduler.h"

namespace frc {
/**
 * A subclass of {@link GenericHID} with {@link Trigger} factories for
 * command-based.
 *
 * @see GenericHID
 */
class CommandGenericHID : public frc::GenericHID {
 public:
  using GenericHID::GenericHID;

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to. Defaults to the
   * CommandScheduler's default loop.
   * @return an event instance representing the button's digital signal attached
   * to the given loop.
   */
  Trigger Button(int button,
                 frc::EventLoop* loop = CommandScheduler::GetInstance()
                                            .GetDefaultButtonLoop()) const;
};
}  // namespace frc
