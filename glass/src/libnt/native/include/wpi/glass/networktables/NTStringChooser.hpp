// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

#include <wpi/nt/NetworkTableInstance.hpp>
#include <wpi/nt/StringArrayTopic.hpp>
#include <wpi/nt/StringTopic.hpp>

#include "wpi/glass/other/StringChooser.hpp"

namespace wpi::glass {

class NTStringChooserModel : public StringChooserModel {
 public:
  static constexpr const char* kType = "String Chooser";

  // path is to the table containing ".type", excluding the trailing /
  explicit NTStringChooserModel(std::string_view path);
  NTStringChooserModel(wpi::nt::NetworkTableInstance inst,
                       std::string_view path);

  const std::string& GetDefault() override { return m_defaultValue; }
  const std::string& GetSelected() override { return m_selectedValue; }
  const std::string& GetActive() override { return m_activeValue; }
  const std::vector<std::string>& GetOptions() override {
    return m_optionsValue;
  }

  void SetSelected(std::string_view val) override;

  void Update() override;
  bool Exists() override;
  bool IsReadOnly() override { return false; }

 private:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::StringSubscriber m_default;
  wpi::nt::StringSubscriber m_selected;
  wpi::nt::StringPublisher m_selectedPub;
  wpi::nt::StringSubscriber m_active;
  wpi::nt::StringArraySubscriber m_options;

  std::string m_defaultValue;
  std::string m_selectedValue;
  std::string m_activeValue;
  std::vector<std::string> m_optionsValue;
};

}  // namespace wpi::glass
