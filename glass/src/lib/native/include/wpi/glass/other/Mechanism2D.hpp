// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <imgui.h>

#include "wpi/glass/Model.hpp"
#include "wpi/glass/View.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/util/function_ref.hpp"

namespace glass {

class MechanismObjectModel;

class MechanismObjectGroup {
 public:
  virtual const char* GetName() const = 0;
  virtual void ForEachObject(
      wpi::function_ref<void(MechanismObjectModel& model)> func) = 0;
};

class MechanismObjectModel : public MechanismObjectGroup {
 public:
  virtual const char* GetType() const = 0;
  virtual ImU32 GetColor() const = 0;

  // line accessors
  virtual double GetWeight() const = 0;
  virtual frc::Rotation2d GetAngle() const = 0;
  virtual units::meter_t GetLength() const = 0;
};

class MechanismRootModel : public MechanismObjectGroup {
 public:
  virtual frc::Translation2d GetPosition() const = 0;
};

class Mechanism2DModel : public Model {
 public:
  virtual frc::Translation2d GetDimensions() const = 0;
  virtual ImU32 GetBackgroundColor() const = 0;
  virtual void ForEachRoot(
      wpi::function_ref<void(MechanismRootModel& model)> func) = 0;
};

void DisplayMechanism2D(Mechanism2DModel* model, const ImVec2& contentSize);
void DisplayMechanism2DSettings(Mechanism2DModel* model);

class Mechanism2DView : public View {
 public:
  explicit Mechanism2DView(Mechanism2DModel* model) : m_model{model} {}

  void Display() override;
  void Settings() override;
  bool HasSettings() override;

 private:
  Mechanism2DModel* m_model;
};

}  // namespace glass
