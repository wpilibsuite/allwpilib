/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/Twine.h>

#include "Button.h"

namespace frc2 {
/**
 * A {@link Button} that uses a {@link NetworkTable} boolean field.
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
                const wpi::Twine& field);

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  NetworkButton(const wpi::Twine& table, const wpi::Twine& field);
};
}  // namespace frc2
