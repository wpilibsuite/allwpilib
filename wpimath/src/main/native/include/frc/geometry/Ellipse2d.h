// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>

#include <gcem.hpp>
#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Transform2d.h"
#include "frc/geometry/Translation2d.h"
#include "units/length.h"
#include "units/math.h"

namespace frc {

/**
 * Represents a 2d ellipse space containing translational, rotational, and
 * scaling components.
 */
class WPILIB_DLLEXPORT Ellipse2d {
 public:
  /**
   * Constructs an ellipse around a center point and two semi-axes, a horizontal
   * and vertical one.
   *
   * @param center The center of the ellipse.
   * @param xSemiAxis The x semi-axis.
   * @param ySemiAxis The y semi-axis.
   */
  constexpr Ellipse2d(const Pose2d& center, units::meter_t xSemiAxis,
                      units::meter_t ySemiAxis)
      : m_center{center}, m_xSemiAxis{xSemiAxis}, m_ySemiAxis{ySemiAxis} {
    if (xSemiAxis <= 0_m || ySemiAxis <= 0_m) {
      throw std::invalid_argument("Ellipse2d semi-axes must be positive");
    }
  }

  /**
   * Constructs a perfectly circular ellipse with the specified radius.
   *
   * @param center The center of the circle.
   * @param radius The radius of the circle.
   */
  constexpr Ellipse2d(const Translation2d& center, double radius)
      : m_center{center, Rotation2d{}},
        m_xSemiAxis{radius},
        m_ySemiAxis{radius} {}

  /**
   * Returns the center of the ellipse.
   *
   * @return The center of the ellipse.
   */
  constexpr const Pose2d& Center() const { return m_center; }

  /**
   * Returns the rotational component of the ellipse.
   *
   * @return The rotational component of the ellipse.
   */
  constexpr const Rotation2d& Rotation() const { return m_center.Rotation(); }

  /**
   * Returns the x semi-axis.
   *
   * @return The x semi-axis.
   */
  constexpr units::meter_t XSemiAxis() const { return m_xSemiAxis; }

  /**
   * Returns the y semi-axis.
   *
   * @return The y semi-axis.
   */
  constexpr units::meter_t YSemiAxis() const { return m_ySemiAxis; }

  /**
   * Returns the focal points of the ellipse. In a perfect circle, this will
   * always return the center.
   *
   * @return The focal points.
   */
  constexpr wpi::array<Translation2d, 2> FocalPoints() const {
    // Major semi-axis
    auto a = units::math::max(m_xSemiAxis, m_ySemiAxis);

    // Minor semi-axis
    auto b = units::math::min(m_xSemiAxis, m_ySemiAxis);  // NOLINT

    auto c = units::math::sqrt(a * a - b * b);

    if (m_xSemiAxis > m_ySemiAxis) {
      return wpi::array{
          (m_center + Transform2d{-c, 0_m, Rotation2d{}}).Translation(),
          (m_center + Transform2d{c, 0_m, Rotation2d{}}).Translation()};
    } else {
      return wpi::array{
          (m_center + Transform2d{0_m, -c, Rotation2d{}}).Translation(),
          (m_center + Transform2d{0_m, c, Rotation2d{}}).Translation()};
    }
  }

  /**
   * Transforms the center of the ellipse and returns the new ellipse.
   *
   * @param other The transform to transform by.
   * @return The transformed ellipse.
   */
  constexpr Ellipse2d TransformBy(const Transform2d& other) const {
    return Ellipse2d{m_center.TransformBy(other), m_xSemiAxis, m_ySemiAxis};
  }

  /**
   * Rotates the center of the ellipse and returns the new ellipse.
   *
   * @param other The rotation to transform by.
   * @return The rotated ellipse.
   */
  constexpr Ellipse2d RotateBy(const Rotation2d& other) const {
    return Ellipse2d{m_center.RotateBy(other), m_xSemiAxis, m_ySemiAxis};
  }

  /**
   * Checks if a point is intersected by this ellipse's circumference.
   *
   * @param point The point to check.
   * @return True, if this ellipse's circumference intersects the point.
   */
  constexpr bool Intersects(const Translation2d& point) const {
    return gcem::abs(1.0 - SolveEllipseEquation(point)) <= 1E-9;
  }

  /**
   * Checks if a point is contained within this ellipse. This is inclusive, if
   * the point lies on the circumference this will return {@code true}.
   *
   * @param point The point to check.
   * @return True, if the point is within or on the ellipse.
   */
  constexpr bool Contains(const Translation2d& point) const {
    return SolveEllipseEquation(point) <= 1.0;
  }

  /**
   * Returns the distance between the perimeter of the ellipse and the point.
   *
   * @param point The point to check.
   * @return The distance (0, if the point is contained by the ellipse)
   */
  units::meter_t Distance(const Translation2d& point) const {
    return Nearest(point).Distance(point);
  }

  /**
   * Returns the nearest point that is contained within the ellipse.
   *
   * @param point The point that this will find the nearest point to.
   * @return A new point that is nearest to {@code point} and contained in the
   *   ellipse.
   */
  Translation2d Nearest(const Translation2d& point) const;

  /**
   * Checks equality between this Ellipse2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const Ellipse2d& other) const {
    return m_center == other.m_center &&
           units::math::abs(m_xSemiAxis - other.m_xSemiAxis) < 1E-9_m &&
           units::math::abs(m_ySemiAxis - other.m_ySemiAxis) < 1E-9_m;
  }

 private:
  Pose2d m_center;
  units::meter_t m_xSemiAxis;
  units::meter_t m_ySemiAxis;

  /**
   * Solves the equation of an ellipse from the given point. This is a helper
   * function used to determine if that point lies inside of or on an ellipse.
   *
   * <pre>
   * (x - h)²/a² + (y - k)²/b² = 1
   * </pre>
   *
   * @param point The point to solve for.
   * @return < 1.0 if the point lies inside the ellipse, == 1.0 if a point lies
   * on the ellipse, and > 1.0 if the point lies outsides the ellipse.
   */
  constexpr double SolveEllipseEquation(const Translation2d& point) const {
    // Rotate the point by the inverse of the ellipse's rotation
    auto rotPoint =
        point.RotateAround(m_center.Translation(), -m_center.Rotation());

    auto x = rotPoint.X() - m_center.X();
    auto y = rotPoint.Y() - m_center.Y();

    return (x * x) / (m_xSemiAxis * m_xSemiAxis) +
           (y * y) / (m_ySemiAxis * m_ySemiAxis);
  }
};

}  // namespace frc

#include "frc/geometry/proto/Ellipse2dProto.h"
#include "frc/geometry/struct/Ellipse2dStruct.h"
