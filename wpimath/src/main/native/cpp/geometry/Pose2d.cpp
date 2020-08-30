/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/geometry/Pose2d.h"

#include <cmath>

#include <wpi/json.h>

using namespace frc;

Pose2d::Pose2d(Translation2d translation, Rotation2d rotation)
    : m_translation(translation), m_rotation(rotation) {}

Pose2d::Pose2d(units::meter_t x, units::meter_t y, Rotation2d rotation)
    : m_translation(x, y), m_rotation(rotation) {}

Pose2d Pose2d::operator+(const Transform2d& other) const {
  return TransformBy(other);
}

Pose2d& Pose2d::operator+=(const Transform2d& other) {
  m_translation += other.Translation().RotateBy(m_rotation);
  m_rotation += other.Rotation();
  return *this;
}

Transform2d Pose2d::operator-(const Pose2d& other) const {
  const auto pose = this->RelativeTo(other);
  return Transform2d(pose.Translation(), pose.Rotation());
}

bool Pose2d::operator==(const Pose2d& other) const {
  return m_translation == other.m_translation && m_rotation == other.m_rotation;
}

bool Pose2d::operator!=(const Pose2d& other) const {
  return !operator==(other);
}

Pose2d Pose2d::TransformBy(const Transform2d& other) const {
  return {m_translation + (other.Translation().RotateBy(m_rotation)),
          m_rotation + other.Rotation()};
}

Pose2d Pose2d::RelativeTo(const Pose2d& other) const {
  const Transform2d transform{other, *this};
  return {transform.Translation(), transform.Rotation()};
}

Pose2d Pose2d::Exp(const Twist2d& twist) const {
  const auto dx = twist.dx;
  const auto dy = twist.dy;
  const auto dtheta = twist.dtheta.to<double>();

  const auto sinTheta = std::sin(dtheta);
  const auto cosTheta = std::cos(dtheta);

  double s, c;
  if (std::abs(dtheta) < 1E-9) {
    s = 1.0 - 1.0 / 6.0 * dtheta * dtheta;
    c = 0.5 * dtheta;
  } else {
    s = sinTheta / dtheta;
    c = (1 - cosTheta) / dtheta;
  }

  const Transform2d transform{Translation2d{dx * s - dy * c, dx * c + dy * s},
                              Rotation2d{cosTheta, sinTheta}};

  return *this + transform;
}

Twist2d Pose2d::Log(const Pose2d& end) const {
  const auto transform = end.RelativeTo(*this);
  const auto dtheta = transform.Rotation().Radians().to<double>();
  const auto halfDtheta = dtheta / 2.0;

  const auto cosMinusOne = transform.Rotation().Cos() - 1;

  double halfThetaByTanOfHalfDtheta;

  if (std::abs(cosMinusOne) < 1E-9) {
    halfThetaByTanOfHalfDtheta = 1.0 - 1.0 / 12.0 * dtheta * dtheta;
  } else {
    halfThetaByTanOfHalfDtheta =
        -(halfDtheta * transform.Rotation().Sin()) / cosMinusOne;
  }

  const Translation2d translationPart =
      transform.Translation().RotateBy(
          {halfThetaByTanOfHalfDtheta, -halfDtheta}) *
      std::hypot(halfThetaByTanOfHalfDtheta, halfDtheta);

  return {translationPart.X(), translationPart.Y(), units::radian_t(dtheta)};
}

void frc::to_json(wpi::json& json, const Pose2d& pose) {
  json = wpi::json{{"translation", pose.Translation()},
                   {"rotation", pose.Rotation()}};
}

void frc::from_json(const wpi::json& json, Pose2d& pose) {
  pose = Pose2d{json.at("translation").get<Translation2d>(),
                json.at("rotation").get<Rotation2d>()};
}
