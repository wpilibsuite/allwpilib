// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DifferentialSample.hpp"

#include "wpi/util/json.hpp"

void wpi::math::to_json(wpi::util::json& json,
                        const DifferentialSample& sample) {
  json = wpi::util::json{{"timestamp", sample.timestamp.value()},
                         {"pose", sample.pose},
                         {"velocity", sample.velocity},
                         {"acceleration", sample.acceleration},
                         {"leftSpeed", sample.leftSpeed.value()},
                         {"rightSpeed", sample.rightSpeed.value()}};
}

void wpi::math::from_json(const wpi::util::json& json,
                          DifferentialSample& sample) {
  sample = DifferentialSample{
      wpi::units::second_t{json.at("timestamp").get<double>()},
      json.at("pose").get<Pose2d>(),
      json.at("velocity").get<ChassisSpeeds>(),
      json.at("acceleration").get<ChassisAccelerations>(),
      wpi::units::meters_per_second_t{json.at("leftSpeed").get<double>()},
      wpi::units::meters_per_second_t{json.at("rightSpeed").get<double>()}};
}
