// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <string>

#include <networktables/NTSendable.h>
#include <networktables/NetworkTableEntry.h>
#include <wpi/SmallVector.h>
#include <wpi/mutex.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {

/**
 * This class is a non-template base class for SendableChooser.
 *
 * It contains static, non-templated variables to avoid their duplication in the
 * template class.
 */
class SendableChooserBase : public nt::NTSendable,
                            public wpi::SendableHelper<SendableChooserBase> {
 public:
  SendableChooserBase();
  ~SendableChooserBase() override = default;

  SendableChooserBase(SendableChooserBase&& oth);
  SendableChooserBase& operator=(SendableChooserBase&& oth);

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
