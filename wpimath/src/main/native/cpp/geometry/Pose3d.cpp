// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Pose3d.h"

#include <cmath>

#include <Eigen/Core>
#include <wpi/json.h>

#include "geometry3d.pb.h"

using namespace frc;

namespace {

/**
 * Applies the hat operator to a rotation vector.
 *
 * It takes a rotation vector and returns the corresponding matrix
 * representation of the Lie algebra element (a 3x3 rotation matrix).
 *
 * @param rotation The rotation vector.
 * @return The rotation vector as a 3x3 rotation matrix.
 */
Eigen::Matrix3d RotationVectorToMatrix(const Eigen::Vector3d& rotation) {
  // Given a rotation vector <a, b, c>,
  //         [ 0 -c  b]
  // Omega = [ c  0 -a]
  //         [-b  a  0]
  return Eigen::Matrix3d{{0.0, -rotation(2), rotation(1)},
                         {rotation(2), 0.0, -rotation(0)},
                         {-rotation(1), rotation(0), 0.0}};
}
}  // namespace

Pose3d::Pose3d(Translation3d translation, Rotation3d rotation)
    : m_translation{std::move(translation)}, m_rotation{std::move(rotation)} {}

Pose3d::Pose3d(units::meter_t x, units::meter_t y, units::meter_t z,
               Rotation3d rotation)
    : m_translation{x, y, z}, m_rotation{std::move(rotation)} {}

Pose3d::Pose3d(const Pose2d& pose)
    : m_translation{pose.X(), pose.Y(), 0_m},
      m_rotation{0_rad, 0_rad, pose.Rotation().Radians()} {}

Pose3d Pose3d::operator+(const Transform3d& other) const {
  return TransformBy(other);
}

Transform3d Pose3d::operator-(const Pose3d& other) const {
  const auto pose = this->RelativeTo(other);
  return Transform3d{pose.Translation(), pose.Rotation()};
}

Pose3d Pose3d::operator*(double scalar) const {
  return Pose3d{m_translation * scalar, m_rotation * scalar};
}

Pose3d Pose3d::operator/(double scalar) const {
  return *this * (1.0 / scalar);
}

Pose3d Pose3d::RotateBy(const Rotation3d& other) const {
  return {m_translation.RotateBy(other), m_rotation.RotateBy(other)};
}

Pose3d Pose3d::TransformBy(const Transform3d& other) const {
  return {m_translation + (other.Translation().RotateBy(m_rotation)),
          other.Rotation() + m_rotation};
}

Pose3d Pose3d::RelativeTo(const Pose3d& other) const {
  const Transform3d transform{other, *this};
  return {transform.Translation(), transform.Rotation()};
}

Pose3d Pose3d::Exp(const Twist3d& twist) const {
  // Implementation from Section 3.2 of https://ethaneade.org/lie.pdf
  Eigen::Vector3d u{twist.dx.value(), twist.dy.value(), twist.dz.value()};
  Eigen::Vector3d rvec{twist.rx.value(), twist.ry.value(), twist.rz.value()};
  Eigen::Matrix3d omega = RotationVectorToMatrix(rvec);
  Eigen::Matrix3d omegaSq = omega * omega;
  double theta = rvec.norm();
  double thetaSq = theta * theta;

  double A;
  double B;
  double C;
  if (std::abs(theta) < 1E-7) {
    // Taylor Expansions around θ = 0
    // A = 1/1! - θ²/3! + θ⁴/5!
    // B = 1/2! - θ²/4! + θ⁴/6!
    // C = 1/3! - θ²/5! + θ⁴/7!
    // sources:
    // A:
    // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D+at+x%3D0
    // B:
    // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-cos%5C%2840%29x%5C%2841%29%2CPower%5Bx%2C2%5D%5D+at+x%3D0
    // C:
    // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D%2CPower%5Bx%2C2%5D%5D+at+x%3D0
    A = 1 - thetaSq / 6 + thetaSq * thetaSq / 120;
    B = 1 / 2.0 - thetaSq / 24 + thetaSq * thetaSq / 720;
    C = 1 / 6.0 - thetaSq / 120 + thetaSq * thetaSq / 5040;
  } else {
    // A = std::sin(θ)/θ
    // B = (1 - std::cos(θ)) / θ²
    // C = (1 - A) / θ²
    A = std::sin(theta) / theta;
    B = (1 - std::cos(theta)) / thetaSq;
    C = (1 - A) / thetaSq;
  }

  Eigen::Matrix3d R = Eigen::Matrix3d::Identity() + A * omega + B * omegaSq;
  Eigen::Matrix3d V = Eigen::Matrix3d::Identity() + B * omega + C * omegaSq;

  auto translation_component = V * u;
  const Transform3d transform{
      Translation3d{units::meter_t{translation_component(0)},
                    units::meter_t{translation_component(1)},
                    units::meter_t{translation_component(2)}},
      Rotation3d{R}};

  return *this + transform;
}

Twist3d Pose3d::Log(const Pose3d& end) const {
  // Implementation from Section 3.2 of https://ethaneade.org/lie.pdf
  const auto transform = end.RelativeTo(*this);

  Eigen::Vector3d u{transform.X().value(), transform.Y().value(),
                    transform.Z().value()};
  Eigen::Vector3d rvec =
      transform.Rotation().GetQuaternion().ToRotationVector();

  Eigen::Matrix3d omega = RotationVectorToMatrix(rvec);
  Eigen::Matrix3d omegaSq = omega * omega;
  double theta = rvec.norm();
  double thetaSq = theta * theta;

  double C;
  if (std::abs(theta) < 1E-7) {
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
    double A = std::sin(theta) / theta;
    double B = (1 - std::cos(theta)) / thetaSq;
    C = (1 - A / (2 * B)) / thetaSq;
  }

  Eigen::Matrix3d V_inv =
      Eigen::Matrix3d::Identity() - 0.5 * omega + C * omegaSq;

  Eigen::Vector3d translation_component = V_inv * u;

  return Twist3d{units::meter_t{translation_component(0)},
                 units::meter_t{translation_component(1)},
                 units::meter_t{translation_component(2)},
                 units::radian_t{rvec(0)},
                 units::radian_t{rvec(1)},
                 units::radian_t{rvec(2)}};
}

Pose2d Pose3d::ToPose2d() const {
  return Pose2d{m_translation.X(), m_translation.Y(), m_rotation.Z()};
}

void frc::to_json(wpi::json& json, const Pose3d& pose) {
  json = wpi::json{{"translation", pose.Translation()},
                   {"rotation", pose.Rotation()}};
}

void frc::from_json(const wpi::json& json, Pose3d& pose) {
  pose = Pose3d{json.at("translation").get<Translation3d>(),
                json.at("rotation").get<Rotation3d>()};
}
