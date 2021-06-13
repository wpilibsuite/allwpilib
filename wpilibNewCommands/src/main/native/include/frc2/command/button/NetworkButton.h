// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

#include "Button.h"

namespace frc2 {
/**
 * A Button that uses a NetworkTable boolean field.
 */
class NetworkButton : public Button {
 public:
  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param entry The entry that is the value.
   */
  explicit NetworkButton(nt::NetworkTableEntry entry);

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  NetworkButton(std::shared_ptr<nt::NetworkTable> table,
                std::string_view field);

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  NetworkButton(std::string_view table, std::string_view field);
};
}  // namespace frc2
