// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Pose2d.h"

#include <cmath>

#include <wpi/json.h>

#include "frc/MathUtil.h"

using namespace frc;

Transform2d Pose2d::operator-(const Pose2d& other) const {
  const auto pose = this->RelativeTo(other);
  return Transform2d{pose.Translation(), pose.Rotation()};
}

Pose2d Pose2d::RelativeTo(const Pose2d& other) const {
  const Transform2d transform{other, *this};
  return {transform.Translation(), transform.Rotation()};
}

Pose2d Pose2d::Exp(const Twist2d& twist) const {
  const auto dx = twist.dx;
  const auto dy = twist.dy;
  const auto dtheta = twist.dtheta.value();

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
  const auto dtheta = transform.Rotation().Radians().value();
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

  return {translationPart.X(), translationPart.Y(), units::radian_t{dtheta}};
}

Pose2d Pose2d::Nearest(std::span<const Pose2d> poses) const {
  return *std::min_element(
      poses.begin(), poses.end(), [this](const Pose2d& a, const Pose2d& b) {
        auto aDistance = this->Translation().Distance(a.Translation());
        auto bDistance = this->Translation().Distance(b.Translation());

        // If the distances are equal sort by difference in rotation
        if (aDistance == bDistance) {
          return std::abs((this->Rotation() - a.Rotation()).Radians().value()) <
                 std::abs((this->Rotation() - b.Rotation()).Radians().value());
        }
        return aDistance < bDistance;
      });
}

Pose2d Pose2d::Nearest(std::initializer_list<Pose2d> poses) const {
  return *std::min_element(
      poses.begin(), poses.end(), [this](const Pose2d& a, const Pose2d& b) {
        auto aDistance = this->Translation().Distance(a.Translation());
        auto bDistance = this->Translation().Distance(b.Translation());

        // If the distances are equal sort by difference in rotation
        if (aDistance == bDistance) {
          return std::abs((this->Rotation() - a.Rotation()).Radians().value()) <
                 std::abs((this->Rotation() - b.Rotation()).Radians().value());
        }
        return aDistance < bDistance;
      });
}

void frc::to_json(wpi::json& json, const Pose2d& pose) {
  json = wpi::json{{"translation", pose.Translation()},
                   {"rotation", pose.Rotation()}};
}

void frc::from_json(const wpi::json& json, Pose2d& pose) {
  pose = Pose2d{json.at("translation").get<Translation2d>(),
                json.at("rotation").get<Rotation2d>()};
}
