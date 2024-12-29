// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

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
   * @deprecated Use a TrapezoidProfile instead
   */
  [[deprecated("Use a TrapezoidProfile instead")]]
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
  }

  void Initialize() override {}

  void Execute() override {
    m_output(m_profile.Calculate(20_ms, m_currentState(), m_goal()));
  }

  void End(bool interrupted) override {}

  bool IsFinished() override { return m_profile.IsFinished(0_s); }

 private:
  frc::TrapezoidProfile<Distance> m_profile;
  std::function<void(State)> m_output;
  std::function<State()> m_goal;
  std::function<State()> m_currentState;
};

}  // namespace frc2
