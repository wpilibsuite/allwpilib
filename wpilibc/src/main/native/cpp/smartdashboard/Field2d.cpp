// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/Field2d.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/tunables/TunableRegistry.hpp"

using namespace wpi;

Field2d::Field2d() {
  m_objects.emplace_back(
      std::make_unique<FieldObject2d>("Robot", FieldObject2d::private_init{}));
  m_objects[0]->SetPose(wpi::math::Pose2d{});
}

Field2d::Field2d(Field2d&& rhs)
    : wpi::tunables::ComplexTunable{std::move(rhs)} {
  std::swap(m_objects, rhs.m_objects);
}

Field2d& Field2d::operator=(Field2d&& rhs) {
  if (this == &rhs) {
    return *this;
  }

  // Base move assignment retains this field's published UID when rhs is
  // unpublished, so replace the child entries that are about to be swapped out.
  const bool republishChildren =
      IsTunablePublished() && !rhs.IsTunablePublished();
  if (republishChildren) {
    RemoveTunableChildren();
  }

  wpi::tunables::ComplexTunable::operator=(std::move(rhs));
  std::swap(m_objects, rhs.m_objects);

  if (republishChildren) {
    PublishTunableChildren();
  }

  return *this;
}

void Field2d::SetRobotPose(const wpi::math::Pose2d& pose) {
  GetRobotObject()->SetPose(pose);
}

void Field2d::SetRobotPose(wpi::units::meters<> x, wpi::units::meters<> y,
                           wpi::math::Rotation2d rotation) {
  GetRobotObject()->SetPose(x, y, rotation);
}

wpi::math::Pose2d Field2d::GetRobotPose() const {
  std::scoped_lock lock(m_mutex);
  return m_objects[0]->GetPose();
}

FieldObject2d* Field2d::GetObject(std::string_view name) {
  FieldObject2d* object;
  std::string objectName;
  {
    std::scoped_lock lock(m_mutex);
    for (auto&& obj : m_objects) {
      if (obj->m_name == name) {
        return obj.get();
      }
    }
    m_objects.emplace_back(
        std::make_unique<FieldObject2d>(name, FieldObject2d::private_init{}));
    object = m_objects.back().get();
    objectName = object->m_name;
  }

  PublishChildTunable(objectName, object->m_posesTunable);
  return object;
}

FieldObject2d* Field2d::GetRobotObject() {
  std::scoped_lock lock(m_mutex);
  return m_objects[0].get();
}

void Field2d::LogTo(wpi::telemetry::TelemetryTable& table) const {
  std::scoped_lock lock(m_mutex);
  for (auto&& obj : m_objects) {
    table.Log(obj->m_name, obj->m_posesTunable.Get());
  }
}

std::string_view Field2d::GetTelemetryType() const {
  return "Field2d";
}

std::string_view Field2d::GetTunableType() const {
  return "Field2d";
}

void Field2d::PublishTunable(wpi::tunables::TunableTable& table) {
  std::vector<std::pair<std::string, FieldObject2d*>> objects;
  {
    std::scoped_lock lock(m_mutex);
    objects.reserve(m_objects.size());
    for (auto&& obj : m_objects) {
      objects.emplace_back(obj->m_name, obj.get());
    }
  }

  for (auto&& [name, obj] : objects) {
    table.Publish(name, obj->m_posesTunable);
  }
}

bool Field2d::IsTunablePublished() const {
  auto info = wpi::tunables::TunableRegistry::GetTunable(GetTunableUid());
  return static_cast<const wpi::tunables::detail::TunableBase*>(info.tunable) ==
         static_cast<const wpi::tunables::detail::TunableBase*>(this);
}

void Field2d::RemoveTunableChildren() {
  for (auto&& obj : m_objects) {
    RemoveChildTunable(obj->m_name);
  }
}

void Field2d::PublishTunableChildren() {
  for (auto&& obj : m_objects) {
    PublishChildTunable(obj->m_name, obj->m_posesTunable);
  }
}
