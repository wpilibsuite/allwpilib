// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <wpi/Twine.h>

#include "frc/buttons/Button.h"

namespace frc {

class NetworkButton : public Button {
 public:
  NetworkButton(const wpi::Twine& tableName, const wpi::Twine& field);
  NetworkButton(std::shared_ptr<nt::NetworkTable> table,
                const wpi::Twine& field);
  ~NetworkButton() override = default;

  NetworkButton(NetworkButton&&) = default;
  NetworkButton& operator=(NetworkButton&&) = default;

  bool Get() override;

 private:
  nt::NetworkTableEntry m_entry;
};

}  // namespace frc
