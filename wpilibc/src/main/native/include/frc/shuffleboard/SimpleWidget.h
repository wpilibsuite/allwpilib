// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <networktables/GenericEntry.h>
#include <networktables/NetworkTable.h>

#include "frc/shuffleboard/ShuffleboardWidget.h"

namespace frc {

class ShuffleboardContainer;

/**
 * A Shuffleboard widget that handles a single data point such as a number or
 * string.
 */
class SimpleWidget final : public ShuffleboardWidget<SimpleWidget> {
 public:
  SimpleWidget(ShuffleboardContainer& parent, std::string_view title);

  /**
   * Gets the NetworkTable entry that contains the data for this widget.
   * The widget owns the entry; the returned pointer's lifetime is the same as
   * that of the widget.
   */
  nt::GenericEntry* GetEntry();

  /**
   * Gets the NetworkTable entry that contains the data for this widget.
   * The widget owns the entry; the returned pointer's lifetime is the same as
   * that of the widget.
   *
   * @param typeString NT type string
   */
  nt::GenericEntry* GetEntry(std::string_view typeString);

  void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                 std::shared_ptr<nt::NetworkTable> metaTable) override;

 private:
  nt::GenericEntry m_entry;
  std::string m_typeString;

  void ForceGenerate();
};

}  // namespace frc
