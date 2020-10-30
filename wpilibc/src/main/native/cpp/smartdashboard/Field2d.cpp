/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/Field2d.h"

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

Field2d::Field2d() {
  m_objects.emplace_back(
      std::make_unique<FieldObject2d>("Robot", FieldObject2d::private_init{}));
  m_objects[0]->SetPose(Pose2d{});
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

FieldObject2d* Field2d::GetObject(const wpi::Twine& name) {
  std::scoped_lock lock(m_mutex);
  std::string nameStr = name.str();
  for (auto&& obj : m_objects) {
    if (obj->m_name == nameStr) return obj.get();
  }
  m_objects.emplace_back(std::make_unique<FieldObject2d>(
      std::move(nameStr), FieldObject2d::private_init{}));
  auto obj = m_objects.back().get();
  if (m_table) obj->m_entry = m_table->GetEntry(obj->m_name);
  return obj;
}

FieldObject2d* Field2d::GetRobotObject() {
  std::scoped_lock lock(m_mutex);
  return m_objects[0].get();
}

void Field2d::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Field2d");
  m_table = builder.GetTable();

  std::scoped_lock lock(m_mutex);
  for (auto&& obj : m_objects) {
    std::scoped_lock lock2(obj->m_mutex);
    obj->m_entry = m_table->GetEntry(obj->m_name);
    obj->UpdateEntry(true);
  }
}
