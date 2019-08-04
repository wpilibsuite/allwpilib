/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <string>

#include <networktables/NetworkTableEntry.h>
#include <wpi/SmallVector.h>
#include <wpi/mutex.h>

#include "frc/smartdashboard/SendableBase.h"

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

  SendableChooserBase(SendableChooserBase&&) = default;
  SendableChooserBase& operator=(SendableChooserBase&&) = default;

 protected:
  static constexpr const char* kDefault = "default";
  static constexpr const char* kOptions = "options";
  static constexpr const char* kSelected = "selected";
  static constexpr const char* kActive = "active";
  static constexpr const char* kInstance = ".instance";

  std::string m_defaultChoice;
  std::string m_selected;
  bool m_haveSelected = false;
  wpi::SmallVector<nt::NetworkTableEntry, 2> m_activeEntries;
  wpi::mutex m_mutex;
  int m_instance;
  static std::atomic_int s_instances;
};

}  // namespace frc
