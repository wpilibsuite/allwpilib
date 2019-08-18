/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

namespace frc {

/**
 * A trapezoid-shaped velocity profile.
 *
 * While this class can be used for a profiled movement from start to finish,
 * the intended usage is to filter a reference's dynamics based on trapezoidal
 * velocity constraints. To compute the reference obeying this constraint, do
 * the following.
 *
 * Initialization:
 * @code{.cpp}
 * TrapezoidalMotionProfile::Constraints constraints{kMaxV, kMaxA};
 * double previousProfiledReference = initialReference;
 * @endcode
 *
 * Run on update:
 * @code{.cpp}
 * TrapezoidalMotionProfile profile{constraints, unprofiledReference,
 *                                  previousProfiledReference};
 * previousProfiledReference = profile.Calculate(timeSincePreviousUpdate);
 * @endcode
 *
 * where `unprofiledReference` is free to change between calls. Note that when
 * the unprofiled reference is within the constraints, `Calculate()` returns the
 * unprofiled reference unchanged.
 *
 * Otherwise, a timer can be started to provide monotonic values for
 * `Calculate()` and to determine when the profile has completed via
 * `IsFinished()`.
 */
template <typename Unit>
class TrapezoidProfile {
 public:
  using UnitPerSec = typename units::unit_t<units::compound_unit<
      typename Unit::unit_type, units::inverse<units::seconds>>>;
  using UnitPerSecSq = typename units::unit_t<
      units::compound_unit<typename Unit::unit_type,
                           units::inverse<units::squared<units::seconds>>>>;

  struct Constraints {
    UnitPerSec maxVelocity{0};
    UnitPerSecSq maxAcceleration{0};

    constexpr Constraints() = default;
    constexpr Constraints(UnitPerSec maxVelocity,
                          UnitPerSecSq maxAcceleration) {
      this->maxVelocity = maxVelocity;
      this->maxAcceleration = maxAcceleration;
    }
  };

  struct State {
    Unit position{0};
    UnitPerSec velocity{0};

    constexpr State() = default;
    constexpr State(Unit position, UnitPerSec velocity) {
      this->position = position;
      this->velocity = velocity;
    }

    bool operator==(const State& rhs) const {
      return position == rhs.position && velocity == rhs.velocity;
    }
    bool operator!=(const State& rhs) const { return !(*this == rhs); }
  };

  /**
   * Construct a TrapezoidProfile.
   *
   * @param constraints The constraints on the profile, like maximum velocity.
   * @param goal        The desired state when the profile is complete.
   * @param initial     The initial state (usually the current state).
   */
  TrapezoidProfile(Constraints constraints, State goal,
                   State initial = State{});

  TrapezoidProfile(TrapezoidProfile&&) = default;
  TrapezoidProfile& operator=(TrapezoidProfile&&) = default;

  /**
   * Calculate the correct position and velocity for the profile at a time t
   * where the beginning of the profile was at time t = 0.
   *
   * @param t The time since the beginning of the profile.
   */
  State Calculate(units::second_t t) const;

  /**
   * Returns the time left until a target distance in the profile is reached.
   *
   * @param target The target distance.
   */
  units::second_t TimeLeftUntil(Unit target) const;

  /**
   * Returns the total time the profile takes to reach the goal.
   */
  units::second_t TotalTime() const { return m_endDeccel; }

  /**
   * Returns true if the profile has reached the goal.
   *
   * The profile has reached the goal if the time since the profile started
   * has exceeded the profile's total time.
   *
   * @param t The time since the beginning of the profile.
   */
  bool IsFinished(units::second_t t) const { return t >= TotalTime(); }

 private:
  /**
   * Returns true if the profile inverted.
   *
   * The profile is inverted if goal position is less than the initial position.
   *
   * @param initial The initial state (usually the current state).
   * @param goal    The desired state when the profile is complete.
   */
  static bool ShouldFlipAcceleration(const State& initial, const State& goal) {
    return initial.position > goal.position;
  }

  // Flip the sign of the velocity and position if the profile is inverted
  State Direct(const State& in) const {
    State result = in;
    result.position *= m_direction;
    result.velocity *= m_direction;
    return result;
  }

  // The direction of the profile, either 1 for forwards or -1 for inverted
  int m_direction;

  Constraints m_constraints;
  State m_initial;
  State m_goal;

  units::second_t m_endAccel;
  units::second_t m_endFullSpeed;
  units::second_t m_endDeccel;
};

}  // namespace frc

#include "frc/trajectory/TrapezoidProfile.inc"
