/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <Eigen/Core>

#include "frc/spline/Spline.h"

namespace frc {
/**
 * Represents a hermite spline of degree 5.
 */
class QuinticHermiteSpline : public Spline<5> {
 public:
  /**
   * Constructs a quintic hermite spline with the specified control vectors.
   * Each control vector contains into about the location of the point, its
   * first derivative, and its second derivative.
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
  QuinticHermiteSpline(std::array<double, 3> xInitialControlVector,
                       std::array<double, 3> xFinalControlVector,
                       std::array<double, 3> yInitialControlVector,
                       std::array<double, 3> yFinalControlVector);

 protected:
  /**
   * Returns the coefficients matrix.
   * @return The coefficients matrix.
   */
  Eigen::Matrix<double, 6, 6> Coefficients() const override {
    return m_coefficients;
  }

 private:
  Eigen::Matrix<double, 6, 6> m_coefficients =
      Eigen::Matrix<double, 6, 6>::Zero();

  /**
   * Returns the hermite basis matrix for quintic hermite spline interpolation.
   * @return The hermite basis matrix for quintic hermite spline interpolation.
   */
  static Eigen::Matrix<double, 6, 6> MakeHermiteBasis() {
    // clang-format off
    static const auto basis = (Eigen::Matrix<double, 6, 6>() <<
      -06.0, -03.0, -00.5, +06.0, -03.0, +00.5,
      +15.0, +08.0, +01.5, -15.0, +07.0, +01.0,
      -10.0, -06.0, -01.5, +10.0, -04.0, +00.5,
      +00.0, +00.0, +00.5, +00.0, +00.0, +00.0,
      +00.0, +01.0, +00.0, +00.0, +00.0, +00.0,
      +01.0, +00.0, +00.0, +00.0, +00.0, +00.0).finished();
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
  static Eigen::Matrix<double, 6, 1> ControlVectorFromArrays(
      std::array<double, 3> initialVector, std::array<double, 3> finalVector) {
    return (Eigen::Matrix<double, 6, 1>() << initialVector[0], initialVector[1],
            initialVector[2], finalVector[0], finalVector[1], finalVector[2])
        .finished();
  }
};
}  // namespace frc
