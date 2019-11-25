/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/trajectory/TrapezoidProfile.h>
#include <units/units.h>

#include "frc2/command/SubsystemBase.h"

namespace frc2 {
/**
 * A subsystem that generates and runs trapezoidal motion profiles
 * automatically.  The user specifies how to use the current state of the motion
 * profile by overriding the `UseState` method.
 */
template <class Distance>
class TrapezoidProfileSubsystem : public SubsystemBase {
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
  TrapezoidProfileSubsystem(Constraints constraints, Distance_t position,
                            units::second_t period = 20_ms)
      : m_constraints(constraints),
        m_state{position, Velocity_t(0)},
        m_goal{position, Velocity_t{0}},
        m_period(period) {}

  void Periodic() override {
    auto profile =
        frc::TrapezoidProfile<Distance>(m_constraints, m_goal, m_state);
    m_state = profile.Calculate(m_period);
    UseState(m_state);
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

 private:
  Constraints m_constraints;
  State m_state;
  State m_goal;
  units::second_t m_period;
};
}  // namespace frc2
