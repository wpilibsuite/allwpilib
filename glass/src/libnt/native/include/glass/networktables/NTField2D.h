/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  void ForEachFieldObjectGroup(
      wpi::function_ref<void(FieldObjectGroupModel& model, wpi::StringRef name)>
          func) override;

 private:
  NetworkTablesHelper m_nt;
  std::string m_path;
  NT_Entry m_name;
  std::string m_nameValue;

  class GroupModel;
  std::vector<std::unique_ptr<GroupModel>> m_groups;
};

}  // namespace glass
