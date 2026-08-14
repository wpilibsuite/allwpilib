// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/util/RawFrame.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::apriltag {

/**
 * Generates a RawFrame containing the AprilTag with the id from family 36h11.
 *
 * @param frame Frame to write the AprilTag image into.
 * @param id AprilTag ID.
 * @return True if the frame was allocated successfully.
 */
WPILIB_DLLEXPORT
bool Generate36h11AprilTagImage(wpi::util::RawFrame* frame, int id);

/**
 * Generates a RawFrame containing the AprilTag with the id from family 16h5.
 *
 * @param frame Frame to write the AprilTag image into.
 * @param id AprilTag ID.
 * @return True if the frame was allocated successfully.
 */
WPILIB_DLLEXPORT
bool Generate16h5AprilTagImage(wpi::util::RawFrame* frame, int id);

}  // namespace wpi::apriltag
