// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Twist2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/Kinematics.h"
#include "frc/kinematics/WheelPositions.h"

namespace frc {

class SE2KinematicPrimitive {
 public:
  explicit constexpr SE2KinematicPrimitive(const Pose2d& pose) : m_pose{pose} {}
  constexpr SE2KinematicPrimitive(const SE2KinematicPrimitive&) = default;

  SE2KinematicPrimitive& operator=(const SE2KinematicPrimitive&) = default;

  constexpr SE2KinematicPrimitive Interpolate(
      const SE2KinematicPrimitive& endValue, double t) const {
    auto twist = m_pose.Log(endValue.m_pose);
    Twist2d scaled{twist.dx * t, twist.dy * t, twist.dtheta * t};

    return SE2KinematicPrimitive{m_pose.Exp(scaled)};
  }

  constexpr const Pose2d& GetPose() const { return m_pose; }

 private:
  Pose2d m_pose;
};

static_assert(frc::WheelPositions<SE2KinematicPrimitive>);

class SE2Kinematics
    : public Kinematics<SE2KinematicPrimitive, SE2KinematicPrimitive> {
 public:
  explicit SE2Kinematics(units::second_t dt) : m_dt{dt} {}

  constexpr ChassisSpeeds ToChassisSpeeds(
      const SE2KinematicPrimitive& wheelSpeeds) const override {
    auto twist = Pose2d{}.Log(wheelSpeeds.GetPose());
    return ChassisSpeeds{twist.dx / m_dt, twist.dy / m_dt, twist.dtheta / m_dt};
  }

  constexpr SE2KinematicPrimitive ToWheelSpeeds(
      const ChassisSpeeds& chassisSpeeds) const override {
    return SE2KinematicPrimitive{
        Pose2d{}.Exp(Twist2d{chassisSpeeds.vx * m_dt, chassisSpeeds.vy * m_dt,
                             chassisSpeeds.omega * m_dt})};
  }

  constexpr Twist2d ToTwist2d(const SE2KinematicPrimitive& start,
                              const SE2KinematicPrimitive& end) const override {
    return start.GetPose().Log(end.GetPose());
  }

 private:
  units::second_t m_dt;
};

}  // namespace frc
