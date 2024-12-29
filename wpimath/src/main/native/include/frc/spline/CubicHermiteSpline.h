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
                     wpi::array<double, 2> yFinalControlVector)
      : m_initialControlVector{xInitialControlVector, yInitialControlVector},
        m_finalControlVector{xFinalControlVector, yFinalControlVector} {
    const auto hermite = MakeHermiteBasis();
    const auto x =
        ControlVectorFromArrays(xInitialControlVector, xFinalControlVector);
    const auto y =
        ControlVectorFromArrays(yInitialControlVector, yFinalControlVector);

    // Populate first two rows with coefficients.
    m_coefficients.template block<1, 4>(0, 0) = hermite * x;
    m_coefficients.template block<1, 4>(1, 0) = hermite * y;

    // Populate Row 2 and Row 3 with the derivatives of the equations above.
    // Then populate row 4 and 5 with the second derivatives.
    for (int i = 0; i < 4; i++) {
      // Here, we are multiplying by (3 - i) to manually take the derivative.
      // The power of the term in index 0 is 3, index 1 is 2 and so on. To find
      // the coefficient of the derivative, we can use the power rule and
      // multiply the existing coefficient by its power.
      m_coefficients.template block<2, 1>(2, i) =
          m_coefficients.template block<2, 1>(0, i) * (3 - i);
    }

    for (int i = 0; i < 3; i++) {
      // Here, we are multiplying by (2 - i) to manually take the derivative.
      // The power of the term in index 0 is 2, index 1 is 1 and so on. To find
      // the coefficient of the derivative, we can use the power rule and
      // multiply the existing coefficient by its power.
      m_coefficients.template block<2, 1>(4, i) =
          m_coefficients.template block<2, 1>(2, i) * (2 - i);
    }
  }

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
  static constexpr Eigen::Matrix4d MakeHermiteBasis() {
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
    return Eigen::Matrix4d{{+2.0, +1.0, -2.0, +1.0},
                           {-3.0, -2.0, +3.0, -1.0},
                           {+0.0, +1.0, +0.0, +0.0},
                           {+1.0, +0.0, +0.0, +0.0}};
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
  static constexpr Eigen::Vector4d ControlVectorFromArrays(
      wpi::array<double, 2> initialVector, wpi::array<double, 2> finalVector) {
    return Eigen::Vector4d{{initialVector[0]},
                           {initialVector[1]},
                           {finalVector[0]},
                           {finalVector[1]}};
  }
};
}  // namespace frc

#include "frc/spline/proto/CubicHermiteSplineProto.h"
#include "frc/spline/struct/CubicHermiteSplineStruct.h"
