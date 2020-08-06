/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/TrajectoryUtil.h"

#include <system_error>

#include <wpi/SmallString.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

using namespace frc;

void TrajectoryUtil::ToPathweaverJson(const Trajectory& trajectory,
                                      const wpi::Twine& path) {
  std::error_code error_code;

  wpi::SmallString<128> buf;
  wpi::raw_fd_ostream output{path.toStringRef(buf), error_code};
  if (error_code) {
    throw std::runtime_error(("Cannot open file: " + path).str());
  }

  wpi::json json = trajectory.States();
  output << json;
  output.flush();
}

Trajectory TrajectoryUtil::FromPathweaverJson(const wpi::Twine& path) {
  std::error_code error_code;

  wpi::SmallString<128> buf;
  wpi::raw_fd_istream input{path.toStringRef(buf), error_code};
  if (error_code) {
    throw std::runtime_error(("Cannot open file: " + path).str());
  }

  wpi::json json;
  input >> json;

  return Trajectory{json.get<std::vector<Trajectory::State>>()};
}

std::string TrajectoryUtil::SerializeTrajectory(const Trajectory& trajectory) {
  wpi::json json = trajectory.States();
  return json.dump();
}

Trajectory TrajectoryUtil::DeserializeTrajectory(
    const wpi::StringRef json_str) {
  wpi::json json = wpi::json::parse(json_str);
  return Trajectory{json.get<std::vector<Trajectory::State>>()};
}
