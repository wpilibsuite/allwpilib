/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
