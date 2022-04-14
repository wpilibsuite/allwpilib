// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>

#include "frc/buttons/Button.h"

namespace frc {

/**
 * A that uses a NetworkTable boolean field.
 *
 * <p>This class is provided by the OldCommands VendorDep
 */
class NetworkButton : public Button {
 public:
  NetworkButton(std::string_view tableName, std::string_view field);
  NetworkButton(std::shared_ptr<nt::NetworkTable> table,
                std::string_view field);
  ~NetworkButton() override = default;

  NetworkButton(NetworkButton&&) = default;
  NetworkButton& operator=(NetworkButton&&) = default;

  bool Get() override;

 private:
  nt::NetworkTableEntry m_entry;
};

}  // namespace frc
