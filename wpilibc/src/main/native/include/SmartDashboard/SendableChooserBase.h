/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <networktables/NetworkTableEntry.h>

#include "SmartDashboard/SendableBase.h"

namespace frc {

/**
 * This class is a non-template base class for SendableChooser.
 *
 * It contains static, non-templated variables to avoid their duplication in the
 * template class.
 */
class SendableChooserBase : public SendableBase {
 public:
  SendableChooserBase();
  ~SendableChooserBase() override = default;

 protected:
  static constexpr const char* kDefault = "default";
  static constexpr const char* kOptions = "options";
  static constexpr const char* kSelected = "selected";

  std::string m_defaultChoice;
  nt::NetworkTableEntry m_selectedEntry;
};

}  // namespace frc
