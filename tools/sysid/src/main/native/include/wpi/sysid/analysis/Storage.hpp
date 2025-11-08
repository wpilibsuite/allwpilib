// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include "wpi/units/time.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/StringMap.hpp"

#include "wpi/sysid/analysis/AnalysisType.hpp"

namespace sysid {

struct MotorData {
  // name of the *motor*, not the test
  std::string name;

  // Data for a single contiguous motor test
  // Timestamps are not necessarily aligned!
  struct Run {
    template <typename T>
      requires std::is_arithmetic_v<T> || units::traits::is_unit_t_v<T>
    struct Sample {
      Sample(units::second_t time, T measurement)
          : time{time}, measurement{measurement} {}
      units::second_t time;
      T measurement;
    };
    std::vector<Sample<units::volt_t>> voltage;
    std::vector<Sample<double>> position;
    std::vector<Sample<double>> velocity;
  };

  std::vector<Run> runs;
};

struct TestData {
  std::string distanceUnit;
  AnalysisType mechanismType;
  wpi::StringMap<MotorData> motorData;
};

/**
 * Represents each data point after it is cleaned and various parameters are
 * calculated.
 */
struct PreparedData {
  /**
   * The timestamp of the data point.
   */
  units::second_t timestamp;

  /**
   * The voltage of the data point.
   */
  double voltage;

  /**
   * The position of the data point.
   */
  double position;

  /**
   * The velocity of the data point.
   */
  double velocity;

  /**
   * The difference in timestamps between this point and the next point.
   */
  units::second_t dt = 0_s;

  /**
   * The acceleration of the data point
   */
  double acceleration = 0.0;

  /**
   * The cosine value of the data point. This is only used for arm data where we
   * take the cosine of the position.
   */
  double cos = 0.0;

  /**
   * The sine value of the data point. This is only used for arm data where we
   * take the sine of the position.
   */
  double sin = 0.0;

  /**
   * Equality operator between PreparedData structs
   *
   * @param rhs Another PreparedData struct
   * @return If the other struct is equal to this one
   */
  constexpr bool operator==(const PreparedData& rhs) const {
    return timestamp == rhs.timestamp && voltage == rhs.voltage &&
           position == rhs.position && velocity == rhs.velocity &&
           dt == rhs.dt && acceleration == rhs.acceleration && cos == rhs.cos;
  }
};

/**
 * Storage used by the analysis manger.
 */
struct Storage {
  /**
   * Dataset for slow (aka quasistatic) test
   */
  std::vector<PreparedData> slowForward;
  std::vector<PreparedData> slowBackward;

  /**
   * Dataset for fast (aka dynamic) test
   */
  std::vector<PreparedData> fastForward;
  std::vector<PreparedData> fastBackward;

  bool empty() const {
    return slowForward.empty() || slowBackward.empty() || fastForward.empty() ||
           fastBackward.empty();
  }
};

}  // namespace sysid
