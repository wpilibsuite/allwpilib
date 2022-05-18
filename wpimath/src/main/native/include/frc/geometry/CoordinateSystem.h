// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/EigenCore.h"
#include "frc/geometry/CoordinateAxis.h"
#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Rotation3d.h"

namespace frc {

/**
 * A helper class that converts Pose3d objects between different standard
 * coordinate frames.
 */
class WPILIB_DLLEXPORT CoordinateSystem {
 public:
  /**
   * Constructs a coordinate system with the given cardinal directions for each
   * axis.
   *
   * @param positiveX The cardinal direction of the positive x-axis.
   * @param positiveY The cardinal direction of the positive y-axis.
   * @param positiveZ The cardinal direction of the positive z-axis.
   * @throws std::domain_error if the coordinate system isn't special orthogonal
   */
  CoordinateSystem(const CoordinateAxis& positiveX,
                   const CoordinateAxis& positiveY,
                   const CoordinateAxis& positiveZ);

  CoordinateSystem(const CoordinateSystem&) = default;
  CoordinateSystem& operator=(const CoordinateSystem&) = default;
  CoordinateSystem(CoordinateSystem&&) = default;
  CoordinateSystem& operator=(CoordinateSystem&&) = default;

  /**
   * Returns an instance of the North-West-Up (NWU) coordinate system.
   *
   * The +X axis is north, the +Y axis is west, and the +Z axis is up.
   */
  static const CoordinateSystem& NWU();

  /**
   * Returns an instance of the East-Down-North (EDN) coordinate system.
   *
   * The +X axis is east, the +Y axis is down, and the +Z axis is north.
   */
  static const CoordinateSystem& EDN();

  /**
   * Returns an instance of the NED coordinate system.
   *
   * The +X axis is north, the +Y axis is east, and the +Z axis is down.
   */
  static const CoordinateSystem& NED();

  /**
   * Converts the given pose from one coordinate system to another.
   *
   * @param pose The pose to convert.
   * @param from The coordinate system the pose starts in.
   * @param to The coordinate system to which to convert.
   * @return The given pose in the desired coordinate system.
   */
  static Pose3d Convert(const Pose3d& pose, const CoordinateSystem& from,
                        const CoordinateSystem& to);

 private:
  // Rotation from this coordinate system to NWU coordinate system
  Rotation3d m_rotation;
};

}  // namespace frc
