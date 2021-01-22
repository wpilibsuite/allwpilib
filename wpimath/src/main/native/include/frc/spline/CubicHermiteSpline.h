// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/array.h>

#include "Eigen/Core"
#include "frc/spline/Spline.h"

namespace frc {
/**
 * Represents a hermite spline of degree 3.
 */
class CubicHermiteSpline : public Spline<3> {
 public:
  /**
   * Constructs a cubic hermite spline with the specified control vectors. Each
   * control vector contains info about the location of the point and its first
   * derivative.
   *
   * @param xInitialControlVector The control vector for the initial point in
   * the x dimension.
   * @param xFinalControlVector The control vector for the final point in
   * the x dimension.
   * @param yInitialControlVector The control vector for the initial point in
   * the y dimension.
   * @param yFinalControlVector The control vector for the final point in
   * the y dimension.
   */
  CubicHermiteSpline(wpi::array<double, 2> xInitialControlVector,
                     wpi::array<double, 2> xFinalControlVector,
                     wpi::array<double, 2> yInitialControlVector,
                     wpi::array<double, 2> yFinalControlVector);

 protected:
  /**
   * Returns the coefficients matrix.
   * @return The coefficients matrix.
   */
  Eigen::Matrix<double, 6, 3 + 1> Coefficients() const override {
    return m_coefficients;
  }

 private:
  Eigen::Matrix<double, 6, 4> m_coefficients =
      Eigen::Matrix<double, 6, 4>::Zero();

  /**
   * Returns the hermite basis matrix for cubic hermite spline interpolation.
   * @return The hermite basis matrix for cubic hermite spline interpolation.
   */
  static Eigen::Matrix<double, 4, 4> MakeHermiteBasis() {
    // clang-format off
    static auto basis = (Eigen::Matrix<double, 4, 4>() <<
     +2.0, +1.0, -2.0, +1.0,
     -3.0, -2.0, +3.0, -1.0,
     +0.0, +1.0, +0.0, +0.0,
     +1.0, +0.0, +0.0, +0.0).finished();
    // clang-format on
    return basis;
  }

  /**
   * Returns the control vector for each dimension as a matrix from the
   * user-provided arrays in the constructor.
   *
   * @param initialVector The control vector for the initial point.
   * @param finalVector The control vector for the final point.
   *
   * @return The control vector matrix for a dimension.
   */
  static Eigen::Vector4d ControlVectorFromArrays(
      wpi::array<double, 2> initialVector, wpi::array<double, 2> finalVector) {
    return (Eigen::Vector4d() << initialVector[0], initialVector[1],
            finalVector[0], finalVector[1])
        .finished();
  }
};
}  // namespace frc
