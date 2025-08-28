// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/Translation3d.h"

#include <wpi/json.h>

void wpimath::to_json(wpi::json& json, const Translation3d& translation) {
  json = wpi::json{{"x", translation.X().value()},
                   {"y", translation.Y().value()},
                   {"z", translation.Z().value()}};
}

void wpimath::from_json(const wpi::json& json, Translation3d& translation) {
  translation = Translation3d{units::meter_t{json.at("x").get<double>()},
                              units::meter_t{json.at("y").get<double>()},
                              units::meter_t{json.at("z").get<double>()}};
}
