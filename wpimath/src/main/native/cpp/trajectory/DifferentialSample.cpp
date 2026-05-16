// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DifferentialSample.hpp"

#include <vector>

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json,
                        const DifferentialSample& sample) {
  json = wpi::util::json::object(
      "timestamp", sample.timestamp.value(), "pose", sample.pose, "velocity",
      sample.velocity, "acceleration", sample.acceleration, "leftSpeed",
      sample.leftSpeed.value(), "rightSpeed", sample.rightSpeed.value());
}

void wpi::math::from_json(const wpi::util::json& json,
                          DifferentialSample& sample) {
  sample = DifferentialSample{
      wpi::units::second_t{json.at("timestamp").get_double()},
      json.at("pose").get<Pose2d>(),
      json.at("velocity").get<ChassisVelocities>(),
      json.at("acceleration").get<ChassisAccelerations>(),
      wpi::units::meters_per_second_t{json.at("leftSpeed").get_double()},
      wpi::units::meters_per_second_t{json.at("rightSpeed").get_double()}};
}

void wpi::math::to_json(wpi::util::json& json,
                        const std::vector<DifferentialSample>& samples) {
  json.set_array();
  for (const auto& sample : samples) {
    json.emplace_back(sample);
  }
}

void wpi::math::from_json(const wpi::util::json& json,
                          std::vector<DifferentialSample>& samples) {
  samples.clear();
  for (const auto& sample : json.get_array()) {
    samples.emplace_back(sample.get<DifferentialSample>());
  }
}
