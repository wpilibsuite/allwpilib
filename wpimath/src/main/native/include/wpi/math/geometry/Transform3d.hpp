// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include "wpi/util/SymbolExports.hpp"

#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"

namespace frc {

class Pose3d;
struct Twist3d;

/**
 * Represents a transformation for a Pose3d in the pose's frame.
 */
class WPILIB_DLLEXPORT Transform3d {
 public:
  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param final The final pose for the transformation.
   */
  constexpr Transform3d(const Pose3d& initial, const Pose3d& final);

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  constexpr Transform3d(Translation3d translation, Rotation3d rotation)
      : m_translation{std::move(translation)},
        m_rotation{std::move(rotation)} {}

  /**
   * Constructs a transform with x, y, and z translations instead of a separate
   * Translation3d.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param z The z component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  constexpr Transform3d(units::meter_t x, units::meter_t y, units::meter_t z,
                        Rotation3d rotation)
      : m_translation{x, y, z}, m_rotation{std::move(rotation)} {}

  /**
   * Constructs a transform with the specified affine transformation matrix.
   *
   * @param matrix The affine transformation matrix.
   * @throws std::domain_error if the affine transformation matrix is invalid.
   */
  constexpr explicit Transform3d(const Eigen::Matrix4d& matrix)
      : m_translation{Eigen::Vector3d{
            {matrix(0, 3)}, {matrix(1, 3)}, {matrix(2, 3)}}},
        m_rotation{
            Eigen::Matrix3d{{matrix(0, 0), matrix(0, 1), matrix(0, 2)},
                            {matrix(1, 0), matrix(1, 1), matrix(1, 2)},
                            {matrix(2, 0), matrix(2, 1), matrix(2, 2)}}} {
    if (matrix(3, 0) != 0.0 || matrix(3, 1) != 0.0 || matrix(3, 2) != 0.0 ||
        matrix(3, 3) != 1.0) {
      throw std::domain_error("Affine transformation matrix is invalid");
    }
  }

  /**
   * Constructs the identity transform -- maps an initial pose to itself.
   */
  constexpr Transform3d() = default;

  /**
   * Constructs a 3D transform from a 2D transform in the X-Y plane.
   **
   * @param transform The 2D transform.
   * @see Rotation3d(Rotation2d)
   * @see Translation3d(Translation2d)
   */
  constexpr explicit Transform3d(const frc::Transform2d& transform)
      : m_translation{frc::Translation3d{transform.Translation()}},
        m_rotation{frc::Rotation3d{transform.Rotation()}} {}

  /**
   * Returns the translation component of the transformation.
   *
   * @return Reference to the translational component of the transform.
   */
  constexpr const Translation3d& Translation() const { return m_translation; }

  /**
   * Returns the X component of the transformation's translation.
   *
   * @return The x component of the transformation's translation.
   */
  constexpr units::meter_t X() const { return m_translation.X(); }

  /**
   * Returns the Y component of the transformation's translation.
   *
   * @return The y component of the transformation's translation.
   */
  constexpr units::meter_t Y() const { return m_translation.Y(); }

  /**
   * Returns the Z component of the transformation's translation.
   *
   * @return The z component of the transformation's translation.
   */
  constexpr units::meter_t Z() const { return m_translation.Z(); }

  /**
   * Returns an affine transformation matrix representation of this
   * transformation.
   */
  constexpr Eigen::Matrix4d ToMatrix() const {
    auto vec = m_translation.ToVector();
    auto mat = m_rotation.ToMatrix();
    return Eigen::Matrix4d{{mat(0, 0), mat(0, 1), mat(0, 2), vec(0)},
                           {mat(1, 0), mat(1, 1), mat(1, 2), vec(1)},
                           {mat(2, 0), mat(2, 1), mat(2, 2), vec(2)},
                           {0.0, 0.0, 0.0, 1.0}};
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return Reference to the rotational component of the transform.
   */
  constexpr const Rotation3d& Rotation() const { return m_rotation; }

  /**
   * Returns a Twist3d of the current transform (pose delta). If b is the output
   * of {@code a.Log()}, then {@code b.Exp()} would yield a.
   *
   * @return The twist that maps the current transform.
   */
  constexpr Twist3d Log() const;

  /**
   * Invert the transformation. This is useful for undoing a transformation.
   *
   * @return The inverted transformation.
   */
  constexpr Transform3d Inverse() const {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    return Transform3d{(-Translation()).RotateBy(-Rotation()), -Rotation()};
  }

  /**
   * Multiplies the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  constexpr Transform3d operator*(double scalar) const {
    return Transform3d(m_translation * scalar, m_rotation * scalar);
  }

  /**
   * Divides the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  constexpr Transform3d operator/(double scalar) const {
    return *this * (1.0 / scalar);
  }

  /**
   * Composes two transformations. The second transform is applied relative to
   * the orientation of the first.
   *
   * @param other The transform to compose with this one.
   * @return The composition of the two transformations.
   */
  constexpr Transform3d operator+(const Transform3d& other) const;

  /**
   * Checks equality between this Transform3d and another object.
   */
  constexpr bool operator==(const Transform3d&) const = default;

 private:
  Translation3d m_translation;
  Rotation3d m_rotation;
};

}  // namespace frc

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Twist3d.hpp"
#include "wpi/math/geometry/detail/RotationVectorToMatrix.hpp"

namespace frc {

constexpr Transform3d::Transform3d(const Pose3d& initial, const Pose3d& final) {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  m_translation = (final.Translation() - initial.Translation())
                      .RotateBy(-initial.Rotation());

  m_rotation = final.Rotation() - initial.Rotation();
}

constexpr Transform3d Transform3d::operator+(const Transform3d& other) const {
  return Transform3d{Pose3d{}, Pose3d{}.TransformBy(*this).TransformBy(other)};
}

constexpr Twist3d Transform3d::Log() const {
  // Implementation from Section 3.2 of https://ethaneade.org/lie.pdf

  auto impl = [this]<typename Matrix3d, typename Vector3d>() -> Twist3d {
    Vector3d u{{m_translation.X().value(), m_translation.Y().value(),
                m_translation.Z().value()}};
    Vector3d rvec = m_rotation.ToVector();
    Matrix3d omega = detail::RotationVectorToMatrix(rvec);
    Matrix3d omegaSq = omega * omega;
    double theta = rvec.norm();
    double thetaSq = theta * theta;

    double C;
    if (gcem::abs(theta) < 1E-7) {
      // Taylor Expansions around θ = 0
      // A = 1/1! - θ²/3! + θ⁴/5!
      // B = 1/2! - θ²/4! + θ⁴/6!
      // C = 1/6 * (1/2 + θ²/5! + θ⁴/7!)
      // sources:
      // A:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D+at+x%3D0
      // B:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-cos%5C%2840%29x%5C%2841%29%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      // C:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-Divide%5BDivide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D%2C2Divide%5B1-cos%5C%2840%29x%5C%2841%29%2CPower%5Bx%2C2%5D%5D%5D%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      C = 1 / 12.0 + thetaSq / 720 + thetaSq * thetaSq / 30240;
    } else {
      // A = std::sin(θ)/θ
      // B = (1 - std::cos(θ)) / θ²
      // C = (1 - A/(2*B)) / θ²
      double A = gcem::sin(theta) / theta;
      double B = (1 - gcem::cos(theta)) / thetaSq;
      C = (1 - A / (2 * B)) / thetaSq;
    }

    Matrix3d V_inv = Matrix3d::Identity() - 0.5 * omega + C * omegaSq;

    Vector3d translation_component = V_inv * u;

    return Twist3d{units::meter_t{translation_component(0)},
                   units::meter_t{translation_component(1)},
                   units::meter_t{translation_component(2)},
                   units::radian_t{rvec(0)},
                   units::radian_t{rvec(1)},
                   units::radian_t{rvec(2)}};
  };

  if (std::is_constant_evaluated()) {
    return impl.template operator()<ct_matrix3d, ct_vector3d>();
  }
  return impl.template operator()<Eigen::Matrix3d, Eigen::Vector3d>();
}

}  // namespace frc

#include "wpi/math/geometry/proto/Transform3dProto.hpp"
#include "wpi/math/geometry/struct/Transform3dStruct.hpp"
