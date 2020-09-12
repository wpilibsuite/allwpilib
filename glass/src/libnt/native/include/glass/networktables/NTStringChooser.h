/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>

#include <ntcore_cpp.h>

#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/other/StringChooser.h"

namespace glass {

class NTStringChooserModel : public StringChooserModel {
 public:
  static constexpr const char* kType = "String Chooser";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTStringChooserModel(wpi::StringRef path);
  NTStringChooserModel(NT_Inst inst, wpi::StringRef path);

  const std::string& GetDefault() override { return m_defaultValue; }
  const std::string& GetSelected() override { return m_selectedValue; }
  const std::string& GetActive() override { return m_activeValue; }
  const std::vector<std::string>& GetOptions() override {
    return m_optionsValue;
  }

  void SetDefault(wpi::StringRef val) override;
  void SetSelected(wpi::StringRef val) override;
  void SetActive(wpi::StringRef val) override;
  void SetOptions(wpi::ArrayRef<std::string> val) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  NetworkTablesHelper m_nt;
  NT_Entry m_default;
  NT_Entry m_selected;
  NT_Entry m_active;
  NT_Entry m_options;

  std::string m_defaultValue;
  std::string m_selectedValue;
  std::string m_activeValue;
  std::vector<std::string> m_optionsValue;
};

}  // namespace glass
