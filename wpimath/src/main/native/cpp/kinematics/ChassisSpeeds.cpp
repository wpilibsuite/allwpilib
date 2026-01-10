// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/ChassisSpeeds.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json, const ChassisSpeeds& speeds) {
  json = wpi::util::json{{"vx", speeds.vx.value()},
                         {"vy", speeds.vy.value()},
                         {"omega", speeds.omega.value()}};
}

void wpi::math::from_json(const wpi::util::json& json, ChassisSpeeds& speeds) {
  speeds = ChassisSpeeds{
      wpi::units::meters_per_second_t{json.at("vx").get<double>()},
      wpi::units::meters_per_second_t{json.at("vy").get<double>()},
      wpi::units::radians_per_second_t{json.at("omega").get<double>()}};
}
