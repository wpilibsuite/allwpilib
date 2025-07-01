// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/Field2d.hpp"

#include <memory>
#include <utility>

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi;

Field2d::Field2d() {
  m_objects.emplace_back(
      std::make_unique<FieldObject2d>("Robot", FieldObject2d::private_init{}));
  m_objects[0]->SetPose(wpi::math::Pose2d{});
}

Field2d::Field2d(Field2d&& rhs) {
  std::swap(m_objects, rhs.m_objects);
}

Field2d& Field2d::operator=(Field2d&& rhs) {
  std::swap(m_objects, rhs.m_objects);
  return *this;
}

void Field2d::SetRobotPose(const wpi::math::Pose2d& pose) {
  std::scoped_lock lock(m_mutex);
  m_objects[0]->SetPose(pose);
}

void Field2d::SetRobotPose(wpi::units::meter_t x, wpi::units::meter_t y,
                           wpi::math::Rotation2d rotation) {
  std::scoped_lock lock(m_mutex);
  m_objects[0]->SetPose(x, y, rotation);
}

wpi::math::Pose2d Field2d::GetRobotPose() const {
  std::scoped_lock lock(m_mutex);
  return m_objects[0]->GetPose();
}

FieldObject2d* Field2d::GetObject(std::string_view name) {
  std::scoped_lock lock(m_mutex);
  for (auto&& obj : m_objects) {
    if (obj->m_name == name) {
      return obj.get();
    }
  }
  m_objects.emplace_back(
      std::make_unique<FieldObject2d>(name, FieldObject2d::private_init{}));
  return m_objects.back().get();
}

FieldObject2d* Field2d::GetRobotObject() {
  std::scoped_lock lock(m_mutex);
  return m_objects[0].get();
}

void Field2d::UpdateTelemetry(wpi::TelemetryTable& table) const {
  std::scoped_lock lock(m_mutex);
  for (auto&& obj : m_objects) {
    std::scoped_lock lock2(obj->m_mutex);
    table.Log(obj->m_name, obj->m_poses);
  }
}

std::string_view Field2d::GetTelemetryType() const {
  return "Field2d";
}
