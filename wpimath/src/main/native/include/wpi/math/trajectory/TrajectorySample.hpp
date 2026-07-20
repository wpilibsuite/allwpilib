// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include "wpi/units/time.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::util {
class json;
}  // namespace wpi::util

namespace wpi::math {

/**
 * Represents a single sample in a trajectory.
 */
class TrajectorySample {
 public:
  /** The time of the sample relative to the trajectory start. */
  wpi::units::second_t time{0.0};

  /** Constructs a default TrajectorySample with all zero values. */
  constexpr TrajectorySample() = default;

  ~TrajectorySample() = default;

  /**
   * Constructs a TrajectorySample.
   *
   * @param time The time of the sample relative to the trajectory start.
   */
  explicit constexpr TrajectorySample(wpi::units::second_t time) : time{time} {}

  /**
   * Checks equality between this TrajectorySample and another.
   *
   * @return True if the samples are equal.
   */
  constexpr bool operator==(const TrajectorySample& other) const = default;
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const TrajectorySample& sample);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, TrajectorySample& sample);

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json,
             const std::vector<TrajectorySample>& samples);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json,
               std::vector<TrajectorySample>& samples);

}  // namespace wpi::math

#include "wpi/math/trajectory/proto/TrajectorySampleProto.hpp"
#include "wpi/math/trajectory/struct/TrajectorySampleStruct.hpp"
