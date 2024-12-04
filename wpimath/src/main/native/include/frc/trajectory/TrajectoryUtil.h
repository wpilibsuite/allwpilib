// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <wpi/SymbolExports.h>

#include "frc/trajectory/Trajectory.h"

namespace frc {
/**
 * Trajectory utilities.
 */
class WPILIB_DLLEXPORT TrajectoryUtil {
 public:
  TrajectoryUtil() = delete;

  /**
   * Exports a Trajectory to a PathWeaver-style JSON file.
   *
   * @param trajectory the trajectory to export
   * @param path the path of the file to export to
   */
  static void ToPathweaverJson(const Trajectory& trajectory,
                               std::string_view path);
  /**
   * Imports a Trajectory from a JSON file exported from PathWeaver.
   *
   * @param path The path of the json file to import from.
   *
   * @return The trajectory represented by the file.
   */
  static Trajectory FromPathweaverJson(std::string_view path);

  /**
   * Deserializes a Trajectory from JSON exported from PathWeaver.
   *
   * @param trajectory the trajectory to export
   *
   * @return the string containing the serialized JSON
   */
  static std::string SerializeTrajectory(const Trajectory& trajectory);

  /**
   * Serializes a Trajectory to PathWeaver-style JSON.
   *
   * @param jsonStr the string containing the serialized JSON
   *
   * @return the trajectory represented by the JSON
   */
  static Trajectory DeserializeTrajectory(std::string_view jsonStr);

 private:
  // Usage reporting for PathWeaver Trajectory instances
  inline static int pathWeaverTrajectoryInstances = 0;
};
}  // namespace frc
