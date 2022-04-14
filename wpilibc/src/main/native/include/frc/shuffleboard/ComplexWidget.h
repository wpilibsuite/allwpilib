// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include <networktables/NetworkTable.h>

#include "frc/shuffleboard/ShuffleboardWidget.h"

namespace wpi {
class Sendable;
class SendableBuilder;
}  // namespace wpi

namespace frc {

class ShuffleboardContainer;

/**
 * A Shuffleboard widget that handles a Sendable object such as a speed
 * controller or sensor.
 */
class ComplexWidget final : public ShuffleboardWidget<ComplexWidget> {
 public:
  ComplexWidget(ShuffleboardContainer& parent, std::string_view title,
                wpi::Sendable& sendable);

  ~ComplexWidget() override;

  void EnableIfActuator() override;

  void DisableIfActuator() override;

  void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                 std::shared_ptr<nt::NetworkTable> metaTable) override;

 private:
  wpi::Sendable& m_sendable;
  std::unique_ptr<wpi::SendableBuilder> m_builder;
};

}  // namespace frc
