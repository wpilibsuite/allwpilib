// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"

namespace wpi::cmd {
/**
 * A Button that uses a NetworkTable boolean field.
 *
 * This class is provided by the NewCommands VendorDep
 */
class NetworkButton : public Trigger {
 public:
  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param topic The boolean topic that contains the value.
   */
  explicit NetworkButton(wpi::nt::BooleanTopic topic);

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param sub The boolean subscriber that provides the value.
   */
  explicit NetworkButton(wpi::nt::BooleanSubscriber sub);

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  NetworkButton(std::shared_ptr<wpi::nt::NetworkTable> table,
                std::string_view field);

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  NetworkButton(std::string_view table, std::string_view field);

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param inst The NetworkTable instance to use
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  NetworkButton(wpi::nt::NetworkTableInstance inst, std::string_view table,
                std::string_view field);
};
}  // namespace wpi::cmd
