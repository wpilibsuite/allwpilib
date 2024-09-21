// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/Field2d.h"

#include <memory>
#include <utility>

#include <networktables/DoubleArrayTopic.h>
#include <networktables/NTSendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

Field2d::Field2d() {
  m_objects.emplace_back(
      std::make_unique<FieldObject2d>("Robot", FieldObject2d::private_init{}));
  m_objects[0]->SetPose(Pose2d{});
  wpi::SendableRegistry::Add(this, "Field");
}

Field2d::Field2d(Field2d&& rhs) : SendableHelper(std::move(rhs)) {
  std::swap(m_table, rhs.m_table);
  std::swap(m_objects, rhs.m_objects);
}

Field2d& Field2d::operator=(Field2d&& rhs) {
  SendableHelper::operator=(std::move(rhs));

  std::swap(m_table, rhs.m_table);
  std::swap(m_objects, rhs.m_objects);

  return *this;
}

void Field2d::SetRobotPose(const Pose2d& pose) {
  std::scoped_lock lock(m_mutex);
  m_objects[0]->SetPose(pose);
}

void Field2d::SetRobotPose(units::meter_t x, units::meter_t y,
                           Rotation2d rotation) {
  std::scoped_lock lock(m_mutex);
  m_objects[0]->SetPose(x, y, rotation);
}

Pose2d Field2d::GetRobotPose() const {
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
  auto obj = m_objects.back().get();
  if (m_table) {
    obj->m_entry = m_table->GetDoubleArrayTopic(obj->m_name).GetEntry({});
  }
  return obj;
}

FieldObject2d* Field2d::GetRobotObject() {
  std::scoped_lock lock(m_mutex);
  return m_objects[0].get();
}

void Field2d::InitSendable(nt::NTSendableBuilder& builder) {
  builder.SetSmartDashboardType("Field2d");

  std::scoped_lock lock(m_mutex);
  m_table = builder.GetTable();
  for (auto&& obj : m_objects) {
    std::scoped_lock lock2(obj->m_mutex);
    obj->m_entry = m_table->GetDoubleArrayTopic(obj->m_name).GetEntry({});
    obj->UpdateEntry(true);
  }
}
