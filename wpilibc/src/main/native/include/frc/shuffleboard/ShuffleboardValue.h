/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <networktables/NetworkTable.h>
#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>

namespace frc {

class ShuffleboardValue {
 public:
  explicit ShuffleboardValue(const wpi::Twine& title) {
    wpi::SmallVector<char, 16> storage;
    m_title = title.toStringRef(storage);
  }

  virtual ~ShuffleboardValue() = default;

  /**
   * Gets the title of this Shuffleboard value.
   */
  wpi::StringRef GetTitle() const { return m_title; }

  /**
   * Builds the entries for this value.
   *
   * @param parentTable The table containing all the data for the parent. Values
   *                    that require a complex entry or table structure should
   *                    call {@code parentTable.getSubtable(getTitle())} to get
   *                    the table to put data into. Values that only use a
   *                    single entry should call
   *                    {@code parentTable.getEntry(getTitle())} to get that
   *                    entry.
   * @param metaTable   The table containing all the metadata for this value and
   *                    its sub-values
   */
  virtual void BuildInto(std::shared_ptr<nt::NetworkTable> parentTable,
                         std::shared_ptr<nt::NetworkTable> metaTable) = 0;

  /**
   * Enables user control of this widget in the Shuffleboard application.
   *
   * This method is package-private to prevent users from enabling control
   * themselves. Has no effect if the sendable is not marked as an actuator with
   * {@link SendableBuilder#setActuator}.
   */
  virtual void EnableIfActuator() {}

  /**
   * Disables user control of this widget in the Shuffleboard application.
   *
   * This method is package-private to prevent users from enabling control
   * themselves. Has no effect if the sendable is not marked as an actuator with
   * {@link SendableBuilder#setActuator}.
   */
  virtual void DisableIfActuator() {}

 private:
  std::string m_title;
};

}  // namespace frc
