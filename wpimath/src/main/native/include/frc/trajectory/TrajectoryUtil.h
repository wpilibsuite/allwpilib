// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "frc/trajectory/Trajectory.h"

namespace frc {
class TrajectoryUtil {
 public:
  TrajectoryUtil() = delete;

  /**
   * Exports a Trajectory to a PathWeaver-style JSON file.
   *
   * @param trajectory the trajectory to export
   * @param path the path of the file to export to
   *
   * @return The interpolated state.
   */
  static void ToPathweaverJson(const Trajectory& trajectory,
                               const wpi::Twine& path);
  /**
   * Imports a Trajectory from a PathWeaver-style JSON file.
   *
   * @param path The path of the json file to import from.
   *
   * @return The trajectory represented by the file.
   */
  static Trajectory FromPathweaverJson(const wpi::Twine& path);

  /**
     * Deserializes a Trajectory from PathWeaver-style JSON.

     * @param json the string containing the serialized JSON

     * @return the trajectory represented by the JSON
     */
  static std::string SerializeTrajectory(const Trajectory& trajectory);

  /**
   * Serializes a Trajectory to PathWeaver-style JSON.

   * @param trajectory the trajectory to export

   * @return the string containing the serialized JSON
   */
  static Trajectory DeserializeTrajectory(wpi::StringRef json_str);
};
}  // namespace frc
