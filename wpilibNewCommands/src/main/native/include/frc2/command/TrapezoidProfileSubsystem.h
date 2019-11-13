/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc2/command/SubsystemBase.h"
#include "frc/trajectory/TrapezoidProfile.h"

#include <units/units.h>

namespace frc2 {
/**
 * A subsystem that generates and runs trapezoidal motion profiles automatically.  The user
 * specifies how to use the current state of the motion profile by overriding the `UseState` method.
 */
template <class Unit>
class TrapezoidProfileSubsystem : public SubsystemBase {
  using State = frc::TrapezoidProfile::State;
  using Constraints = frc::TrapezoidProfile::Constraints;
 public:
  /**
   * Creates a new TrapezoidProfileSubsystem.
   *
   * @param constraints     The constraints (maximum velocity and acceleration) for the profiles.
   * @param initialPosition The initial position of the controller mechanism when the subsystem
   *                        is constructed.
   * @param period          The period of the main robot loop, in seconds.
   */
  TrapezoidProfileSubsystem(Constraints constraints,
                            units::unit_t<Unit> position,
                            units::second_t period = .02_s)
                            : m_constraints(constraints),
                              m_state{units::meter_t(position.template to<double>()), 0_mps},
                              m_period(period) {}

  void Periodic() override {
    auto profile = frc::TrapezoidProfile(m_constraints, GetGoal(), m_state);
    m_state = profile.Calculate(m_period);
    UseState(m_state);
  }

  /**
   * Users should override this to return the goal state for the subsystem's motion profile.
   *
   * @return The goal state for the subsystem's motion profile.
   */
  virtual State GetGoal() = 0;

 protected:

  /**
   * Users should override this to consume the current state of the motion profile.
   *
   * @param state The current state of the motion profile.
   */
  virtual void UseState(State state) = 0;

 private:
  Constraints m_constraints;
  State m_state;
  units::second_t m_period;
};
}
