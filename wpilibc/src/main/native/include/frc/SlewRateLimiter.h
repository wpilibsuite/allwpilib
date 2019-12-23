/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/Timer.h>

#include <algorithm>

#include <units/units.h>

namespace frc {
/**
 * A class that limits the rate of change of an input value.  Useful for
 * implementing voltage, setpoint, and/or output ramps.  A slew-rate limit
 * is most appropriate when the quantity being controlled is a velocity or
 * a voltage; when controlling a position, consider using a TrapezoidProfile
 * instead.
 *
 * @see TrapezoidProfile
 */
template <class Unit>
class SlewRateLimiter {
  using Unit_t = units::unit_t<Unit>;
  using Rate = units::compound_unit<Unit, units::inverse<units::seconds>>;
  using Rate_t = units::unit_t<Rate>;

 public:
  /**
   * Creates a new SlewRateLimiter with the given rate limit and initial value.
   *
   * @param rateLimit The rate-of-change limit.
   * @param initialValue The initial value of the input.
   */
  explicit SlewRateLimiter(Rate_t rateLimit, Unit_t initialValue = Unit_t{0})
      : m_rateLimit{rateLimit}, m_prevVal{initialValue} {
    m_timer.Start();
  }

  /**
   * Filters the input to limit its slew rate.
   *
   * @param input The input value whose slew rate is to be limited.
   * @return The filtered value, which will not change faster than the slew
   * rate.
   */
  Unit_t Calculate(Unit_t input) {
    m_prevVal += std::clamp(input - m_prevVal, -m_rateLimit * m_timer.Get(),
                            m_rateLimit * m_timer.Get());
    m_timer.Reset();
    return m_prevVal;
  }

  /**
   * Resets the slew rate limiter to the specified value; ignores the rate limit
   * when doing so.
   *
   * @param value The value to reset to.
   */
  void Reset(Unit_t value) {
    m_timer.Reset();
    m_prevVal = value;
  }

 private:
  frc2::Timer m_timer;
  Rate_t m_rateLimit;
  Unit_t m_prevVal;
};
}  // namespace frc
