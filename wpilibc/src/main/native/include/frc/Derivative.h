/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cassert>
#include <type_traits>

#include <units/units.h>
#include <wpi/circular_buffer.h>

namespace frc {
template <class Unit>
class Derivative {
 public:
  using Unit_t = units::unit_t<Unit>;
  using Velocity = units::compound_unit<Unit, units::inverse<units::seconds>>;
  using Velocity_t = units::unit_t<Velocity>;

  Derivative(size_t taps, units::second_t period)
      : m_buffer(taps), m_taps(taps), m_period(period) {
    assert(taps >= 2);
  }

  Velocity_t Calculate(Unit_t input) {
    m_buffer.push_front(input);
    return (m_buffer.front() - m_buffer.back()) / ((m_taps - 1) * m_period);
  }

  void Reset() { m_buffer.Reset(); }

 private:
  wpi::circular_buffer<Unit_t> m_buffer;
  size_t m_taps;
  units::second_t m_period;
};
}  // namespace frc
