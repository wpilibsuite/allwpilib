// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/base.hpp"
#include "wpi/units/time.hpp"

namespace wpi::math {

/**
 * Represents a single sample in an exponential profile trajectory.
 */
template <class Distance, class Input>
class ExponentialProfileSample {
 public:
  using Distance_t = wpi::units::unit_t<Distance>;
  using Velocity =
      wpi::units::compound_unit<Distance,
                                wpi::units::inverse<wpi::units::seconds>>;
  using Velocity_t = wpi::units::unit_t<Velocity>;
  using Acceleration =
      wpi::units::compound_unit<Velocity,
                                wpi::units::inverse<wpi::units::seconds>>;
  using Acceleration_t = wpi::units::unit_t<Acceleration>;
  using Input_t = wpi::units::unit_t<Input>;

  /// The time of the sample relative to the profile start.
  wpi::units::second_t time = 0_s;

  /// The position at this sample.
  Distance_t position{0};

  /// The velocity at this sample.
  Velocity_t velocity{0};

  /// The acceleration at this sample.
  Acceleration_t acceleration{0};

  /// The input applied at this sample.
  Input_t input{0};

  /** Constructs a default ExponentialProfileSample with all zero values. */
  constexpr ExponentialProfileSample() = default;

  /**
   * Constructs an ExponentialProfileSample.
   *
   * @param time The time of the sample relative to the profile start.
   * @param position The position at this sample.
   * @param velocity The velocity at this sample.
   * @param acceleration The acceleration at this sample.
   * @param input The input applied at this sample.
   */
  constexpr ExponentialProfileSample(wpi::units::second_t time,
                                     Distance_t position, Velocity_t velocity,
                                     Acceleration_t acceleration, Input_t input)
      : time{time},
        position{position},
        velocity{velocity},
        acceleration{acceleration},
        input{input} {}

  /**
   * Checks equality between this ExponentialProfileSample and another.
   *
   * @return True if the samples are equal.
   */
  constexpr bool operator==(const ExponentialProfileSample&) const = default;
};

}  // namespace wpi::math
