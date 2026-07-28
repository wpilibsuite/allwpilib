// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string_view>

#include <imgui.h>

#include "wpi/glass/Model.hpp"
#include "wpi/glass/View.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/util/function_ref.hpp"

namespace wpi::glass {

class FieldObjectModel : public Model {
 public:
  virtual const char* GetName() const = 0;

  virtual std::span<const wpi::math::Pose2d> GetPoses() = 0;
  virtual void SetPoses(std::span<const wpi::math::Pose2d> poses) = 0;
  virtual void SetPose(size_t i, wpi::math::Pose2d pose) = 0;
  virtual void SetPosition(size_t i, wpi::math::Translation2d pos) = 0;
  virtual void SetRotation(size_t i, wpi::math::Rotation2d rot) = 0;
};

class Field2DModel : public Model {
 public:
  virtual FieldObjectModel* AddFieldObject(std::string_view name) = 0;
  virtual void RemoveFieldObject(std::string_view name) = 0;
  virtual void ForEachFieldObject(
      wpi::util::function_ref<void(FieldObjectModel& model,
                                   std::string_view name)>
          func) = 0;
};

void DisplayField2D(Field2DModel* model, const ImVec2& contentSize);
void DisplayField2DSettings(Field2DModel* model);

class Field2DView : public View {
 public:
  explicit Field2DView(Field2DModel* model) : m_model{model} {}

  void Display() override;
  void Settings() override;
  bool HasSettings() override;

 private:
  Field2DModel* m_model;
};

}  // namespace wpi::glass
