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
 * Represents a hermite spline of degree 5.
 */
class WPILIB_DLLEXPORT QuinticHermiteSpline : public Spline<5> {
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
  QuinticHermiteSpline(wpi::array<double, 3> xInitialControlVector,
                       wpi::array<double, 3> xFinalControlVector,
                       wpi::array<double, 3> yInitialControlVector,
                       wpi::array<double, 3> yFinalControlVector);

  /**
   * Returns the coefficients matrix.
   * @return The coefficients matrix.
   */
  Matrixd<6, 6> Coefficients() const override { return m_coefficients; }

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
  Matrixd<6, 6> m_coefficients = Matrixd<6, 6>::Zero();

  ControlVector m_initialControlVector;
  ControlVector m_finalControlVector;

  /**
   * Returns the hermite basis matrix for quintic hermite spline interpolation.
   * @return The hermite basis matrix for quintic hermite spline interpolation.
   */
  static Matrixd<6, 6> MakeHermiteBasis() {
    // Given P(i), P'(i), P"(i), P(i+1), P'(i+1), P"(i+1), the control vectors,
    // we want to find the coefficients of the spline
    // P(t) = a₅t⁵ + a₄t⁴ + a₃t³ + a₂t² + a₁t + a₀.
    //
    // P(i)    = P(0)  = a₀
    // P'(i)   = P'(0) = a₁
    // P''(i)  = P"(0) = 2a₂
    // P(i+1)  = P(1)  = a₅ + a₄ + a₃ + a₂ + a₁ + a₀
    // P'(i+1) = P'(1) = 5a₅ + 4a₄ + 3a₃ + 2a₂ + a₁
    // P"(i+1) = P"(1) = 20a₅ + 12a₄ + 6a₃ + 2a₂
    //
    // [P(i)   ] = [ 0  0  0  0  0  1][a₅]
    // [P'(i)  ] = [ 0  0  0  0  1  0][a₄]
    // [P"(i)  ] = [ 0  0  0  2  0  0][a₃]
    // [P(i+1) ] = [ 1  1  1  1  1  1][a₂]
    // [P'(i+1)] = [ 5  4  3  2  1  0][a₁]
    // [P"(i+1)] = [20 12  6  2  0  0][a₀]
    //
    // To solve for the coefficients, we can invert the 6x6 matrix and move it
    // to the other side of the equation.
    //
    // [a₅] = [ -6.0  -3.0  -0.5   6.0  -3.0   0.5][P(i)   ]
    // [a₄] = [ 15.0   8.0   1.5 -15.0   7.0  -1.0][P'(i)  ]
    // [a₃] = [-10.0  -6.0  -1.5  10.0  -4.0   0.5][P"(i)  ]
    // [a₂] = [  0.0   0.0   0.5   0.0   0.0   0.0][P(i+1) ]
    // [a₁] = [  0.0   1.0   0.0   0.0   0.0   0.0][P'(i+1)]
    // [a₀] = [  1.0   0.0   0.0   0.0   0.0   0.0][P"(i+1)]

    static const Matrixd<6, 6> basis{
        {-06.0, -03.0, -00.5, +06.0, -03.0, +00.5},
        {+15.0, +08.0, +01.5, -15.0, +07.0, -01.0},
        {-10.0, -06.0, -01.5, +10.0, -04.0, +00.5},
        {+00.0, +00.0, +00.5, +00.0, +00.0, +00.0},
        {+00.0, +01.0, +00.0, +00.0, +00.0, +00.0},
        {+01.0, +00.0, +00.0, +00.0, +00.0, +00.0}};
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
  static Vectord<6> ControlVectorFromArrays(wpi::array<double, 3> initialVector,
                                            wpi::array<double, 3> finalVector) {
    return Vectord<6>{initialVector[0], initialVector[1], initialVector[2],
                      finalVector[0],   finalVector[1],   finalVector[2]};
  }
};
}  // namespace frc
