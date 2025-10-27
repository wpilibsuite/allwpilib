// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <stdexcept>

#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Transform2d.h"
#include "frc/geometry/Translation2d.h"
#include "units/length.h"
#include "units/math.h"

namespace frc {

/**
 * Represents a 2d rectangular space containing translational, rotational, and
 * scaling components.
 */
class WPILIB_DLLEXPORT Rectangle2d {
 public:
  /**
   * Constructs a rectangle at the specified position with the specified width
   * and height.
   *
   * @param center The position (translation and rotation) of the rectangle.
   * @param xWidth The x size component of the rectangle, in unrotated
   * coordinate frame.
   * @param yWidth The y size component of the rectangle, in unrotated
   * coordinate frame.
   */
  constexpr Rectangle2d(const Pose2d& center, units::meter_t xWidth,
                        units::meter_t yWidth)
      : m_center{center}, m_xWidth{xWidth}, m_yWidth{yWidth} {
    if (xWidth < 0_m || yWidth < 0_m) {
      throw std::invalid_argument(
          "Rectangle2d dimensions cannot be less than 0!");
    }
  }

  /**
   * Creates an unrotated rectangle from the given corners. The corners should
   * be diagonally opposite of each other.
   *
   * @param cornerA The first corner of the rectangle.
   * @param cornerB The second corner of the rectangle.
   */
  constexpr Rectangle2d(const Translation2d& cornerA,
                        const Translation2d& cornerB)
      : m_center{(cornerA + cornerB) / 2.0, Rotation2d{}},
        m_xWidth{units::math::abs(cornerA.X() - cornerB.X())},
        m_yWidth{units::math::abs(cornerA.Y() - cornerB.Y())} {}

  /**
   * Returns the center of the rectangle.
   *
   * @return The center of the rectangle.
   */
  constexpr const Pose2d& Center() const { return m_center; }

  /**
   * Returns the rotational component of the rectangle.
   *
   * @return The rotational component of the rectangle.
   */
  constexpr const Rotation2d& Rotation() const { return m_center.Rotation(); }

  /**
   * Returns the x size component of the rectangle.
   *
   * @return The x size component of the rectangle.
   */
  constexpr units::meter_t XWidth() const { return m_xWidth; }

  /**
   * Returns the y size component of the rectangle.
   *
   * @return The y size component of the rectangle.
   */
  constexpr units::meter_t YWidth() const { return m_yWidth; }

  /**
   * Transforms the center of the rectangle and returns the new rectangle.
   *
   * @param other The transform to transform by.
   * @return The transformed rectangle
   */
  constexpr Rectangle2d TransformBy(const Transform2d& other) const {
    return Rectangle2d{m_center.TransformBy(other), m_xWidth, m_yWidth};
  }

  /**
   * Rotates the center of the rectangle and returns the new rectangle.
   *
   * @param other The rotation to transform by.
   * @return The rotated rectangle.
   */
  constexpr Rectangle2d RotateBy(const Rotation2d& other) const {
    return Rectangle2d{m_center.RotateBy(other), m_xWidth, m_yWidth};
  }

  /**
   * Checks if a point is intersected by the rectangle's perimeter.
   *
   * @param point The point to check.
   * @return True, if the rectangle's perimeter intersects the point.
   */
  constexpr bool Intersects(const Translation2d& point) const {
    // Move the point into the rectangle's coordinate frame
    auto pointInRect = point - m_center.Translation();
    pointInRect = pointInRect.RotateBy(-m_center.Rotation());

    if (units::math::abs(units::math::abs(pointInRect.X()) - m_xWidth / 2.0) <=
        1E-9_m) {
      // Point rests on left/right perimeter
      return units::math::abs(pointInRect.Y()) <= m_yWidth / 2.0;
    } else if (units::math::abs(units::math::abs(pointInRect.Y()) -
                                m_yWidth / 2.0) <= 1E-9_m) {
      // Point rests on top/bottom perimeter
      return units::math::abs(pointInRect.X()) <= m_xWidth / 2.0;
    }

    return false;
  }

  /**
   * Checks if a point is contained within the rectangle. This is inclusive, if
   * the point lies on the perimeter it will return true.
   *
   * @param point The point to check.
   * @return True, if the rectangle contains the point or the perimeter
   * intersects the point.
   */
  constexpr bool Contains(const Translation2d& point) const {
    // Rotate the point into the rectangle's coordinate frame
    auto rotPoint =
        point.RotateAround(m_center.Translation(), -m_center.Rotation());

    // Check if within bounding box
    return rotPoint.X() >= (m_center.X() - m_xWidth / 2.0) &&
           rotPoint.X() <= (m_center.X() + m_xWidth / 2.0) &&
           rotPoint.Y() >= (m_center.Y() - m_yWidth / 2.0) &&
           rotPoint.Y() <= (m_center.Y() + m_yWidth / 2.0);
  }

  /**
   * Returns the distance between the perimeter of the rectangle and the point.
   *
   * @param point The point to check.
   * @return The distance (0, if the point is contained by the rectangle)
   */
  constexpr units::meter_t Distance(const Translation2d& point) const {
    return Nearest(point).Distance(point);
  }

  /**
   * Returns the nearest point that is contained within the rectangle.
   *
   * @param point The point that this will find the nearest point to.
   * @return A new point that is nearest to {@code point} and contained in the
   * rectangle.
   */
  constexpr Translation2d Nearest(const Translation2d& point) const {
    // Check if already in rectangle
    if (Contains(point)) {
      return point;
    }

    // Rotate the point by the inverse of the rectangle's rotation
    auto rotPoint =
        point.RotateAround(m_center.Translation(), -m_center.Rotation());

    // Find nearest point
    rotPoint =
        Translation2d{std::clamp(rotPoint.X(), m_center.X() - m_xWidth / 2.0,
                                 m_center.X() + m_xWidth / 2.0),
                      std::clamp(rotPoint.Y(), m_center.Y() - m_yWidth / 2.0,
                                 m_center.Y() + m_yWidth / 2.0)};

    // Undo rotation
    return rotPoint.RotateAround(m_center.Translation(), m_center.Rotation());
  }

  /**
   * Checks equality between this Rectangle2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const Rectangle2d& other) const {
    return m_center == other.m_center &&
           units::math::abs(m_xWidth - other.m_xWidth) < 1E-9_m &&
           units::math::abs(m_yWidth - other.m_yWidth) < 1E-9_m;
  }

 private:
  Pose2d m_center;
  units::meter_t m_xWidth;
  units::meter_t m_yWidth;
};

}  // namespace frc

#include "frc/geometry/proto/Rectangle2dProto.h"
#include "frc/geometry/struct/Rectangle2dStruct.h"
