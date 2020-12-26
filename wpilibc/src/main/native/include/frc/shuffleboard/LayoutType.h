// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/StringRef.h>

namespace frc {

/**
 * Represents the type of a layout in Shuffleboard. Using this is preferred over
 * specifying raw strings, to avoid typos and having to know or look up the
 * exact string name for a desired layout.
 *
 * @see BuiltInLayouts the built-in layout types
 */
class LayoutType {
 public:
  explicit constexpr LayoutType(const char* layoutName)
      : m_layoutName(layoutName) {}
  ~LayoutType() = default;

  /**
   * Gets the string type of the layout as defined by that layout in
   * Shuffleboard.
   */
  wpi::StringRef GetLayoutName() const;

 private:
  const char* m_layoutName;
};

}  // namespace frc
