// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ntcore_cpp.h>
#include <wpi/StringRef.h>

#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/Field2D.h"

namespace glass {

class NTField2DModel : public Field2DModel {
 public:
  static constexpr const char* kType = "Field2d";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTField2DModel(wpi::StringRef path);
  NTField2DModel(NT_Inst inst, wpi::StringRef path);
  ~NTField2DModel() override;

  const char* GetPath() const { return m_path.c_str(); }
  const char* GetName() const { return m_nameValue.c_str(); }

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override;

  FieldObjectModel* AddFieldObject(wpi::StringRef name) override;
  void RemoveFieldObject(wpi::StringRef name) override;
  void ForEachFieldObject(
      wpi::function_ref<void(FieldObjectModel& model, wpi::StringRef name)>
          func) override;

 private:
  NetworkTablesHelper m_nt;
  std::string m_path;
  NT_Entry m_name;
  std::string m_nameValue;

  class ObjectModel;
  std::vector<std::unique_ptr<ObjectModel>> m_objects;
};

}  // namespace glass
