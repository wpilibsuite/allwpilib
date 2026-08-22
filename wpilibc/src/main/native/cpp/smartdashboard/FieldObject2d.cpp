// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/FieldObject2d.hpp"

#include <stdint.h>

#include <span>
#include <utility>
#include <vector>

using namespace wpi;

FieldObject2d::FieldObject2d(FieldObject2d&& rhs)
    : m_name{std::move(rhs.m_name)},
      m_posesTunable{m_mutex, std::move(rhs.m_posesTunable)} {}

FieldObject2d& FieldObject2d::operator=(FieldObject2d&& rhs) {
  if (this == &rhs) {
    return *this;
  }
  std::scoped_lock lock(m_mutex, rhs.m_mutex);
  m_name = std::move(rhs.m_name);
  m_posesTunable = std::move(rhs.m_posesTunable);
  return *this;
}

void FieldObject2d::SetPose(const wpi::math::Pose2d& pose) {
  SetPoses({pose});
}

void FieldObject2d::SetPose(wpi::units::meters<> x, wpi::units::meters<> y,
                            wpi::math::Rotation2d rotation) {
  SetPoses({{x, y, rotation}});
}

wpi::math::Pose2d FieldObject2d::GetPose() const {
  auto poses = m_posesTunable.Get();
  if (poses.empty()) {
    return {};
  }
  return poses[0];
}

void FieldObject2d::SetPoses(std::span<const wpi::math::Pose2d> poses) {
  m_posesTunable.Set(poses);
}

void FieldObject2d::SetPoses(std::initializer_list<wpi::math::Pose2d> poses) {
  SetPoses({poses.begin(), poses.end()});
}

std::vector<wpi::math::Pose2d> FieldObject2d::GetPoses() const {
  return m_posesTunable.Get();
}

std::span<const wpi::math::Pose2d> FieldObject2d::GetPoses(
    wpi::util::SmallVectorImpl<wpi::math::Pose2d>& out) const {
  auto poses = m_posesTunable.Get();
  out.assign(poses.begin(), poses.end());
  return out;
}

FieldObject2d::PosesTunable::PosesTunable(wpi::util::mutex& mutex)
    : m_mutex{&mutex} {}

FieldObject2d::PosesTunable::PosesTunable(wpi::util::mutex& mutex,
                                          PosesTunable&& rhs)
    : Base{std::move(rhs)}, m_mutex{&mutex} {}

FieldObject2d::PosesTunable& FieldObject2d::PosesTunable::operator=(
    PosesTunable&& rhs) {
  Base::operator=(std::move(rhs));
  return *this;
}

std::vector<wpi::math::Pose2d> FieldObject2d::PosesTunable::Get() const {
  std::scoped_lock lock(*m_mutex);
  return Base::Get();
}

void FieldObject2d::PosesTunable::Set(
    std::span<const wpi::math::Pose2d> poses) {
  std::scoped_lock lock(*m_mutex);
  Base::Set(poses);
}

void FieldObject2d::PosesTunable::Set(std::vector<wpi::math::Pose2d> poses) {
  std::scoped_lock lock(*m_mutex);
  Base::Set(std::move(poses));
}

size_t FieldObject2d::PosesTunable::GetStructSize() const {
  std::scoped_lock lock(*m_mutex);
  return Base::GetStructSize();
}

bool FieldObject2d::PosesTunable::UnpackStruct(std::span<const uint8_t> data) {
  std::scoped_lock lock(*m_mutex);
  return Base::UnpackStruct(data);
}

void FieldObject2d::PosesTunable::PackStruct(std::span<uint8_t> buf) const {
  std::scoped_lock lock(*m_mutex);
  Base::PackStruct(buf);
}
