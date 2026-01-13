// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/ChassisAccelerations.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json,
                        const ChassisAccelerations& accel) {
  json = wpi::util::json{{"ax", accel.ax.value()},
                         {"ay", accel.ay.value()},
                         {"alpha", accel.alpha.value()}};
}

void wpi::math::from_json(const wpi::util::json& json,
                          ChassisAccelerations& accel) {
  accel = ChassisAccelerations{
      units::meters_per_second_squared_t{json.at("ax").get<double>()},
      units::meters_per_second_squared_t{json.at("ay").get<double>()},
      units::radians_per_second_squared_t{json.at("alpha").get<double>()}};
}
