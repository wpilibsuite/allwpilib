// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/EigenCore.h"
#include "frc/spline/Spline.h"

namespace frc {
/**
 * Represents a hermite spline of degree 3.
 */
class WPILIB_DLLEXPORT CubicHermiteSpline : public Spline<3> {
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

  /**
   * Returns the coefficients matrix.
   * @return The coefficients matrix.
   */
  Matrixd<6, 3 + 1> Coefficients() const override { return m_coefficients; }

  /**
   * Returns the initial control vector that created this spline.
   *
   * @return The initial control vector that created this spline.
   */
  const ControlVector& GetInitialControlVector() const override {
    return m_initialControlVector;
  }

  /**
   * Returns the final control vector that created this spline.
   *
   * @return The final control vector that created this spline.
   */
  const ControlVector& GetFinalControlVector() const override {
    return m_finalControlVector;
  }

 private:
  Matrixd<6, 4> m_coefficients = Matrixd<6, 4>::Zero();

  ControlVector m_initialControlVector;
  ControlVector m_finalControlVector;

  /**
   * Returns the hermite basis matrix for cubic hermite spline interpolation.
   * @return The hermite basis matrix for cubic hermite spline interpolation.
   */
  static Eigen::Matrix4d MakeHermiteBasis() {
    // Given P(i), P'(i), P(i+1), P'(i+1), the control vectors, we want to find
    // the coefficients of the spline P(t) = a₃t³ + a₂t² + a₁t + a₀.
    //
    // P(i)    = P(0)  = a₀
    // P'(i)   = P'(0) = a₁
    // P(i+1)  = P(1)  = a₃ + a₂ + a₁ + a₀
    // P'(i+1) = P'(1) = 3a₃ + 2a₂ + a₁
    //
    // [P(i)   ] = [0 0 0 1][a₃]
    // [P'(i)  ] = [0 0 1 0][a₂]
    // [P(i+1) ] = [1 1 1 1][a₁]
    // [P'(i+1)] = [3 2 1 0][a₀]
    //
    // To solve for the coefficients, we can invert the 4x4 matrix and move it
    // to the other side of the equation.
    //
    // [a₃] = [ 2  1 -2  1][P(i)   ]
    // [a₂] = [-3 -2  3 -1][P'(i)  ]
    // [a₁] = [ 0  1  0  0][P(i+1) ]
    // [a₀] = [ 1  0  0  0][P'(i+1)]

    static const Eigen::Matrix4d basis{{+2.0, +1.0, -2.0, +1.0},
                                       {-3.0, -2.0, +3.0, -1.0},
                                       {+0.0, +1.0, +0.0, +0.0},
                                       {+1.0, +0.0, +0.0, +0.0}};
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
    return Eigen::Vector4d{initialVector[0], initialVector[1], finalVector[0],
                           finalVector[1]};
  }
};
}  // namespace frc
