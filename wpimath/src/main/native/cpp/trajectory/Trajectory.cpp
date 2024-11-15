// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/Trajectory.h"

#include <wpi/json.h>

using namespace frc;

void frc::to_json(wpi::json& json, const Trajectory::State& state) {
  json = wpi::json{{"time", state.t.value()},
                   {"velocity", state.velocity.value()},
                   {"acceleration", state.acceleration.value()},
                   {"pose", state.pose},
                   {"curvature", state.curvature.value()}};
}

void frc::from_json(const wpi::json& json, Trajectory::State& state) {
  state.pose = json.at("pose").get<Pose2d>();
  state.t = units::second_t{json.at("time").get<double>()};
  state.velocity =
      units::meters_per_second_t{json.at("velocity").get<double>()};
  state.acceleration =
      units::meters_per_second_squared_t{json.at("acceleration").get<double>()};
  state.curvature = units::curvature_t{json.at("curvature").get<double>()};
}
