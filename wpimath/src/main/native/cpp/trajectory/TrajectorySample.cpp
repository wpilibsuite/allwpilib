// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/TrajectorySample.hpp"

#include <vector>

#include "wpi/units/time.hpp"
#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json, const TrajectorySample& sample) {
  json = wpi::util::json::object("time", sample.time.value());
}

void wpi::math::from_json(const wpi::util::json& json,
                          TrajectorySample& sample) {
  sample = TrajectorySample{wpi::units::second_t{json.at("time").get_number()}};
}

void wpi::math::to_json(wpi::util::json& json,
                        const std::vector<TrajectorySample>& samples) {
  json.set_array();
  for (const auto& sample : samples) {
    json.emplace_back(sample);
  }
}

void wpi::math::from_json(const wpi::util::json& json,
                          std::vector<TrajectorySample>& samples) {
  samples.clear();
  for (const auto& sample : json.get_array()) {
    samples.emplace_back(sample.get<TrajectorySample>());
  }
}
