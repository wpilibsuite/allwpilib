// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <gcem.hpp>
#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Rotation3d.h"

namespace frc {

/**
 * A class representing a coordinate system axis within the NWU coordinate
 * system.
 */
class WPILIB_DLLEXPORT CoordinateAxis {
 public:
  /**
   * Constructs a coordinate system axis within the NWU coordinate system and
   * normalizes it.
   *
   * @param x The x component.
   * @param y The y component.
   * @param z The z component.
   */
  constexpr CoordinateAxis(double x, double y, double z) {
    double norm = gcem::hypot(x, y, z);
    m_axis = Eigen::Vector3d{{x / norm, y / norm, z / norm}};
  }

  CoordinateAxis(const CoordinateAxis&) = default;
  CoordinateAxis& operator=(const CoordinateAxis&) = default;

  CoordinateAxis(CoordinateAxis&&) = default;
  CoordinateAxis& operator=(CoordinateAxis&&) = default;

  /**
   * Returns a coordinate axis corresponding to +X in the NWU coordinate system.
   */
  static constexpr CoordinateAxis N() { return CoordinateAxis{1.0, 0.0, 0.0}; }

  /**
   * Returns a coordinate axis corresponding to -X in the NWU coordinate system.
   */
  static constexpr CoordinateAxis S() { return CoordinateAxis{-1.0, 0.0, 0.0}; }

  /**
   * Returns a coordinate axis corresponding to -Y in the NWU coordinate system.
   */
  static constexpr CoordinateAxis E() { return CoordinateAxis{0.0, -1.0, 0.0}; }

  /**
   * Returns a coordinate axis corresponding to +Y in the NWU coordinate system.
   */
  static constexpr CoordinateAxis W() { return CoordinateAxis{0.0, 1.0, 0.0}; }

  /**
   * Returns a coordinate axis corresponding to +Z in the NWU coordinate system.
   */
  static constexpr CoordinateAxis U() { return CoordinateAxis{0.0, 0.0, 1.0}; }

  /**
   * Returns a coordinate axis corresponding to -Z in the NWU coordinate system.
   */
  static constexpr CoordinateAxis D() { return CoordinateAxis{0.0, 0.0, -1.0}; }

 private:
  friend class CoordinateSystem;

  Eigen::Vector3d m_axis;
};

}  // namespace frc
