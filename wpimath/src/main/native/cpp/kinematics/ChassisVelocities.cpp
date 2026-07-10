// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/ChassisVelocities.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json,
                        const ChassisVelocities& velocities) {
  json = wpi::util::json::object("vx", velocities.vx.value(), "vy",
                                 velocities.vy.value(), "omega",
                                 velocities.omega.value());
}

void wpi::math::from_json(const wpi::util::json& json,
                          ChassisVelocities& velocities) {
  velocities = ChassisVelocities{
      wpi::units::meters_per_second_t{json.at("vx").get_double()},
      wpi::units::meters_per_second_t{json.at("vy").get_double()},
      wpi::units::radians_per_second_t{json.at("omega").get_double()}};
}
