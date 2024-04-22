// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc/Timer.h>
#include <frc/trajectory/TrapezoidProfile.h>

#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/Requirements.h"

namespace frc2 {
/**
 * A command that runs a TrapezoidProfile.  Useful for smoothly controlling
 * mechanism motion.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see TrapezoidProfile
 */
template <class Distance>
class TrapezoidProfileCommand
    : public CommandHelper<Command, TrapezoidProfileCommand<Distance>> {
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using State = typename frc::TrapezoidProfile<Distance>::State;

 public:
  /**
   * Creates a new TrapezoidProfileCommand that will execute the given
   * TrapezoidalProfile. Output will be piped to the provided consumer function.
   *
   * @param profile      The motion profile to execute.
   * @param output       The consumer for the profile output.
   * @param goal The supplier for the desired state
   * @param currentState The current state
   * @param requirements The list of requirements.
   */
  TrapezoidProfileCommand(frc::TrapezoidProfile<Distance> profile,
                          std::function<void(State)> output,
                          std::function<State()> goal,
                          std::function<State()> currentState,
                          Requirements requirements = {})
      : m_profile(profile),
        m_output(output),
        m_goal(goal),
        m_currentState(currentState) {
    this->AddRequirements(requirements);
    m_newAPI = true;
  }

  /**
   * Creates a new TrapezoidProfileCommand that will execute the given
   * TrapezoidalProfile. Output will be piped to the provided consumer function.
   *
   * @param profile      The motion profile to execute.
   * @param output       The consumer for the profile output.
   * @param requirements The list of requirements.
   * @deprecated The new constructor allows you to pass in a supplier for
   * desired and current state. This allows you to change goals at runtime.
   */
  [[deprecated(
      "The new constructor allows you to pass in a supplier for desired and "
      "current state. This allows you to change goals at runtime.")]]
  TrapezoidProfileCommand(frc::TrapezoidProfile<Distance> profile,
                          std::function<void(State)> output,
                          Requirements requirements = {})
      : m_profile(profile), m_output(output) {
    this->AddRequirements(requirements);
    m_newAPI = false;
  }

  void Initialize() override { m_timer.Restart(); }

  void Execute() override {
    m_output(m_profile.Calculate(m_timer.Get(), m_currentState(), m_goal()));
  }

  void End(bool interrupted) override { m_timer.Stop(); }

  bool IsFinished() override {
    return m_timer.HasElapsed(m_profile.TotalTime());
  }

 private:
  frc::TrapezoidProfile<Distance> m_profile;
  std::function<void(State)> m_output;
  std::function<State()> m_goal;
  std::function<State()> m_currentState;
  bool m_newAPI;  // TODO: Remove
  frc::Timer m_timer;
};

}  // namespace frc2
