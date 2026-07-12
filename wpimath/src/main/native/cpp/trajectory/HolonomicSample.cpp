// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/HolonomicSample.hpp"

#include <vector>

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json, const HolonomicSample& sample) {
  json = wpi::util::json::object("time", sample.time.value(), "pose",
                                 sample.pose, "velocity", sample.velocity,
                                 "acceleration", sample.acceleration);
}

void wpi::math::from_json(const wpi::util::json& json,
                          HolonomicSample& sample) {
  sample = HolonomicSample{wpi::units::second_t{json.at("time").get_number()},
                           json.at("pose").get<Pose2d>(),
                           json.at("velocity").get<ChassisVelocities>(),
                           json.at("acceleration").get<ChassisAccelerations>()};
}

void wpi::math::to_json(wpi::util::json& json,
                        const std::vector<HolonomicSample>& samples) {
  json.set_array();
  for (const auto& sample : samples) {
    json.emplace_back(sample);
  }
}

void wpi::math::from_json(const wpi::util::json& json,
                          std::vector<HolonomicSample>& samples) {
  samples.clear();
  for (const auto& sample : json.get_array()) {
    samples.emplace_back(sample.get<HolonomicSample>());
  }
}
