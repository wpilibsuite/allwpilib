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
#include <networktables/NetworkTableValue.h>
#include <wpi/StringMap.h>
#include <wpi/Twine.h>

#include "frc/shuffleboard/ShuffleboardComponentBase.h"

namespace frc {

class ShuffleboardContainer;

/**
 * A generic component in Shuffleboard.
 *
 * @tparam Derived the self type
 */
template <typename Derived>
class ShuffleboardComponent : public ShuffleboardComponentBase {
 public:
  ShuffleboardComponent(ShuffleboardContainer& parent, const wpi::Twine& title,
                        const wpi::Twine& type = "");

  virtual ~ShuffleboardComponent() = default;

  /**
   * Sets custom properties for this component. Property names are
   * case-sensitive and whitespace-insensitive (capitalization and spaces do not
   * matter).
   *
   * @param properties the properties for this component
   * @return this component
   */
  Derived& WithProperties(
      const wpi::StringMap<std::shared_ptr<nt::Value>>& properties);

  /**
   * Sets the position of this component in the tab. This has no effect if this
   * component is inside a layout.
   *
   * If the position of a single component is set, it is recommended to set the
   * positions of <i>all</i> components inside a tab to prevent Shuffleboard
   * from automatically placing another component there before the one with the
   * specific position is sent.
   *
   * @param columnIndex the column in the tab to place this component
   * @param rowIndex    the row in the tab to place this component
   * @return this component
   */
  Derived& WithPosition(int columnIndex, int rowIndex);

  /**
   * Sets the size of this component in the tab. This has no effect if this
   * component is inside a layout.
   *
   * @param width  how many columns wide the component should be
   * @param height how many rows high the component should be
   * @return this component
   */
  Derived& WithSize(int width, int height);
};

}  // namespace frc

#include "frc/shuffleboard/ShuffleboardComponent.inc"
