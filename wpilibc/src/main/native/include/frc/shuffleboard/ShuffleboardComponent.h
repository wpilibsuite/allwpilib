// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableValue.h>
#include <wpi/StringMap.h>

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
  /**
   * Constructs a ShuffleboardComponent.
   *
   * @param parent The parent container.
   * @param title The component title.
   * @param type The component type.
   */
  ShuffleboardComponent(ShuffleboardContainer& parent, std::string_view title,
                        std::string_view type = "")
      : ShuffleboardValue(title),
        ShuffleboardComponentBase(parent, title, type) {}

  /**
   * Sets custom properties for this component. Property names are
   * case-sensitive and whitespace-insensitive (capitalization and spaces do not
   * matter).
   *
   * @param properties the properties for this component
   * @return this component
   */
  Derived& WithProperties(const wpi::StringMap<nt::Value>& properties) {
    m_properties = properties;
    m_metadataDirty = true;
    return *static_cast<Derived*>(this);
  }

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
  Derived& WithPosition(int columnIndex, int rowIndex) {
    m_column = columnIndex;
    m_row = rowIndex;
    m_metadataDirty = true;
    return *static_cast<Derived*>(this);
  }

  /**
   * Sets the size of this component in the tab. This has no effect if this
   * component is inside a layout.
   *
   * @param width  how many columns wide the component should be
   * @param height how many rows high the component should be
   * @return this component
   */
  Derived& WithSize(int width, int height) {
    m_width = width;
    m_height = height;
    m_metadataDirty = true;
    return *static_cast<Derived*>(this);
  }
};

}  // namespace frc
