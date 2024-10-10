// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/trajectory/TrapezoidProfile.h>
#include <units/time.h>

#include "frc2/command/SubsystemBase.h"

namespace frc2 {
/**
 * A subsystem that generates and runs trapezoidal motion profiles
 * automatically.  The user specifies how to use the current state of the motion
 * profile by overriding the `UseState` method.
 *
 * This class is provided by the NewCommands VendorDep
 * @deprecated Use a TrapezoidProfile instead
 */
template <class Distance>
class [[deprecated("Use a TrapezoidProfile instead")]] TrapezoidProfileSubsystem
    : public SubsystemBase {
  using Distance_t = units::unit_t<Distance>;
  using Velocity =
      units::compound_unit<Distance, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;
  using State = typename frc::TrapezoidProfile<Distance>::State;
  using Constraints = typename frc::TrapezoidProfile<Distance>::Constraints;

 public:
  /**
   * Creates a new TrapezoidProfileSubsystem.
   *
   * @param constraints     The constraints (maximum velocity and acceleration)
   * for the profiles.
   * @param initialPosition The initial position of the controller mechanism
   * when the subsystem is constructed.
   * @param period          The period of the main robot loop, in seconds.
   */
  explicit TrapezoidProfileSubsystem(Constraints constraints,
                                     Distance_t initialPosition = Distance_t{0},
                                     units::second_t period = 20_ms)
      : m_profile(constraints),
        m_state{initialPosition, Velocity_t(0)},
        m_goal{initialPosition, Velocity_t{0}},
        m_period(period) {}

  void Periodic() override {
    m_state = m_profile.Calculate(m_period, m_state, m_goal);
    if (m_enabled) {
      UseState(m_state);
    }
  }

  /**
   * Sets the goal state for the subsystem.
   *
   * @param goal The goal state for the subsystem's motion profile.
   */
  void SetGoal(State goal) { m_goal = goal; }

  /**
   * Sets the goal state for the subsystem.  Goal velocity assumed to be zero.
   *
   * @param goal The goal position for the subsystem's motion profile.
   */
  void SetGoal(Distance_t goal) { m_goal = State{goal, Velocity_t(0)}; }

 protected:
  /**
   * Users should override this to consume the current state of the motion
   * profile.
   *
   * @param state The current state of the motion profile.
   */
  virtual void UseState(State state) = 0;

  /**
   * Enable the TrapezoidProfileSubsystem's output.
   */
  void Enable() { m_enabled = true; }

  /**
   * Disable the TrapezoidProfileSubsystem's output.
   */
  void Disable() { m_enabled = false; }

 private:
  frc::TrapezoidProfile<Distance> m_profile;
  State m_state;
  State m_goal;
  units::second_t m_period;
  bool m_enabled{false};
};
}  // namespace frc2
