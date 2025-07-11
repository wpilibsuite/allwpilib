// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/FieldObject2d.h"

#include <utility>
#include <vector>

#include "frc/trajectory/Trajectory.h"

using namespace frc;

FieldObject2d::FieldObject2d(FieldObject2d&& rhs) {
  std::swap(m_name, rhs.m_name);
  std::swap(m_poses, rhs.m_poses);
}

FieldObject2d& FieldObject2d::operator=(FieldObject2d&& rhs) {
  std::swap(m_name, rhs.m_name);
  std::swap(m_poses, rhs.m_poses);

  return *this;
}

void FieldObject2d::SetPose(const Pose2d& pose) {
  SetPoses({pose});
}

void FieldObject2d::SetPose(units::meter_t x, units::meter_t y,
                            Rotation2d rotation) {
  SetPoses({{x, y, rotation}});
}

Pose2d FieldObject2d::GetPose() const {
  std::scoped_lock lock(m_mutex);
  if (m_poses.empty()) {
    return {};
  }
  return m_poses[0];
}

void FieldObject2d::SetPoses(std::span<const Pose2d> poses) {
  std::scoped_lock lock(m_mutex);
  m_poses.assign(poses.begin(), poses.end());
}

void FieldObject2d::SetPoses(std::initializer_list<Pose2d> poses) {
  SetPoses({poses.begin(), poses.end()});
}

void FieldObject2d::SetTrajectory(const Trajectory& trajectory) {
  std::scoped_lock lock(m_mutex);
  m_poses.clear();
  m_poses.reserve(trajectory.States().size());
  for (auto&& state : trajectory.States()) {
    m_poses.push_back(state.pose);
  }
}

std::vector<Pose2d> FieldObject2d::GetPoses() const {
  std::scoped_lock lock(m_mutex);
  return std::vector<Pose2d>(m_poses.begin(), m_poses.end());
}

std::span<const Pose2d> FieldObject2d::GetPoses(
    wpi::SmallVectorImpl<Pose2d>& out) const {
  std::scoped_lock lock(m_mutex);
  out.assign(m_poses.begin(), m_poses.end());
  return out;
}
