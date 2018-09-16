/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <networktables/NetworkTable.h>
#include <wpi/Twine.h>

#include "frc/shuffleboard/ShuffleboardWidget.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableBuilderImpl.h"

namespace frc {

class Sendable;
class ShuffleboardContainer;

/**
 * A Shuffleboard widget that handles a {@link Sendable} object such as a speed
 * controller or sensor.
 */
class ComplexWidget final : public ShuffleboardWidget<ComplexWidget> {
 public:
  ComplexWidget(ShuffleboardContainer& parent, const wpi::Twine& title,
                Sendable& sendable);

  void EnableIfActuator() override;

  void DisableIfActuator() override;

  void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                 std::shared_ptr<nt::NetworkTable> metaTable) override;

 private:
  Sendable& m_sendable;
  SendableBuilderImpl m_builder;
  bool m_builderInit = false;
};

}  // namespace frc
