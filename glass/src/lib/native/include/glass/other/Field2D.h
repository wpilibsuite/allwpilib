/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <imgui.h>
#include <wpi/STLExtras.h>
#include <wpi/StringRef.h>

#include "glass/Model.h"
#include "glass/View.h"

namespace glass {

class DataSource;

class FieldObjectModel : public Model {
 public:
  virtual DataSource* GetXData() = 0;
  virtual DataSource* GetYData() = 0;
  virtual DataSource* GetRotationData() = 0;

  virtual void SetPose(double x, double y, double rot) = 0;
  virtual void SetPosition(double x, double y) = 0;
  virtual void SetRotation(double rot) = 0;
};

class FieldObjectGroupModel : public Model {
 public:
  virtual void ForEachFieldObject(
      wpi::function_ref<void(FieldObjectModel& model)> func) = 0;
};

class Field2DModel : public Model {
 public:
  virtual void ForEachFieldObjectGroup(
      wpi::function_ref<void(FieldObjectGroupModel& model, wpi::StringRef name)>
          func) = 0;
};

void DisplayField2D(Field2DModel* model, const ImVec2& contentSize);
void DisplayField2DSettings(Field2DModel* model);

class Field2DView : public View {
 public:
  explicit Field2DView(Field2DModel* model) : m_model{model} {}

  void Display() override;

 private:
  Field2DModel* m_model;
};

}  // namespace glass
