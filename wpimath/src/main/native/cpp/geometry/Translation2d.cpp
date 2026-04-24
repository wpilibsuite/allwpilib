// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Translation2d.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json,
                        const Translation2d& translation) {
  json.set_object();
  json["x"] = translation.X().value();
  json["y"] = translation.Y().value();
}

void wpi::math::from_json(const wpi::util::json& json,
                          Translation2d& translation) {
  translation = Translation2d{wpi::units::meter_t{json.at("x").get_number()},
                              wpi::units::meter_t{json.at("y").get_number()}};
}
